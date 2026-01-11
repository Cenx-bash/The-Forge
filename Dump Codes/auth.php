<?php
require_once __DIR__ . '/SessionManager.php';
require_once __DIR__ . '/../config/database.php';

class Auth {
    private $db;
    private $session;
    private $pepper = "your-secret-pepper-string-here"; // Store in environment variable
    
    public function __construct() {
        $this->db = Database::getInstance();
        $this->session = new SessionManager();
    }
    
    public function register($email, $password, $name, $phone = null) {
        // Validate input
        if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
            return ['success' => false, 'message' => 'Invalid email format'];
        }
        
        if (strlen($password) < 8) {
            return ['success' => false, 'message' => 'Password must be at least 8 characters'];
        }
        
        // Check if user exists
        $stmt = $this->db->prepare("SELECT id FROM users WHERE email = ?");
        $stmt->execute([$email]);
        
        if ($stmt->rowCount() > 0) {
            return ['success' => false, 'message' => 'Email already registered'];
        }
        
        // Hash password with pepper
        $pwd_peppered = hash_hmac("sha256", $password, $this->pepper);
        $hashed_password = password_hash($pwd_peppered, PASSWORD_ARGON2ID, [
            'memory_cost' => 65536,
            'time_cost' => 4,
            'threads' => 3
        ]);
        
        // Generate verification token
        $verification_token = bin2hex(random_bytes(32));
        
        try {
            $stmt = $this->db->prepare("
                INSERT INTO users (email, password, name, phone, verification_token, created_at) 
                VALUES (?, ?, ?, ?, ?, NOW())
            ");
            
            $stmt->execute([$email, $hashed_password, $name, $phone, $verification_token]);
            $userId = $this->db->lastInsertId();
            
            // Send verification email (implement email function)
            $this->sendVerificationEmail($email, $verification_token);
            
            // Log the user in immediately
            $this->session->set('user_id', $userId);
            $this->session->set('user_email', $email);
            $this->session->set('user_name', $name);
            $this->session->set('user_role', 'user');
            $this->session->set('logged_in', true);
            
            // Generate CSRF token
            $this->generateCSRFToken();
            
            return ['success' => true, 'message' => 'Registration successful'];
            
        } catch (PDOException $e) {
            error_log("Registration error: " . $e->getMessage());
            return ['success' => false, 'message' => 'Registration failed. Please try again.'];
        }
    }
    
    public function login($email, $password, $remember = false) {
        // Rate limiting
        $loginAttempts = $this->session->get('login_attempts', 0);
        if ($loginAttempts >= 5) {
            return ['success' => false, 'message' => 'Too many login attempts. Try again later.'];
        }
        
        $stmt = $this->db->prepare("
            SELECT id, email, password, name, role, is_verified, two_factor_enabled 
            FROM users 
            WHERE email = ? AND is_active = 1
        ");
        $stmt->execute([$email]);
        $user = $stmt->fetch();
        
        if (!$user) {
            $this->session->set('login_attempts', $loginAttempts + 1);
            return ['success' => false, 'message' => 'Invalid credentials'];
        }
        
        // Verify password with pepper
        $pwd_peppered = hash_hmac("sha256", $password, $this->pepper);
        
        if (!password_verify($pwd_peppered, $user['password'])) {
            $this->session->set('login_attempts', $loginAttempts + 1);
            return ['success' => false, 'message' => 'Invalid credentials'];
        }
        
        if (!$user['is_verified']) {
            return ['success' => false, 'message' => 'Please verify your email address'];
        }
        
        // Check if password needs rehash
        if (password_needs_rehash($user['password'], PASSWORD_ARGON2ID)) {
            $newHash = password_hash($pwd_peppered, PASSWORD_ARGON2ID);
            $updateStmt = $this->db->prepare("UPDATE users SET password = ? WHERE id = ?");
            $updateStmt->execute([$newHash, $user['id']]);
        }
        
        // Set session
        $this->session->set('user_id', $user['id']);
        $this->session->set('user_email', $user['email']);
        $this->session->set('user_name', $user['name']);
        $this->session->set('user_role', $user['role']);
        $this->session->set('logged_in', true);
        $this->session->remove('login_attempts');
        
        // Generate CSRF token
        $this->generateCSRFToken();
        
        // Set remember me cookie
        if ($remember) {
            $this->setRememberMeToken($user['id']);
        }
        
        // Log login activity
        $this->logActivity($user['id'], 'login');
        
        // Handle 2FA if enabled
        if ($user['two_factor_enabled']) {
            $this->session->set('2fa_required', true);
            $this->session->set('temp_user_id', $user['id']);
            return ['success' => true, 'requires_2fa' => true];
        }
        
        return ['success' => true, 'requires_2fa' => false];
    }
    
    public function logout() {
        if ($this->session->get('logged_in')) {
            $this->logActivity($this->session->get('user_id'), 'logout');
        }
        
        // Clear remember me token
        if (isset($_COOKIE['remember_token'])) {
            $this->clearRememberMeToken($_COOKIE['remember_token']);
            setcookie('remember_token', '', time() - 3600, '/', '', true, true);
        }
        
        $this->session->destroy();
    }
    
    public function isLoggedIn() {
        if ($this->session->get('logged_in')) {
            return true;
        }
        
        // Check remember me token
        if (isset($_COOKIE['remember_token'])) {
            return $this->validateRememberMeToken($_COOKIE['remember_token']);
        }
        
        return false;
    }
    
    public function requireLogin() {
        if (!$this->isLoggedIn()) {
            $this->session->setFlash('error', 'Please login to access this page');
            header('Location: login.php');
            exit();
        }
    }
    
    public function requireRole($role) {
        $this->requireLogin();
        
        $userRole = $this->session->get('user_role');
        if ($userRole !== $role) {
            $this->session->setFlash('error', 'Insufficient permissions');
            header('Location: dashboard.php');
            exit();
        }
    }
    
    public function generateCSRFToken() {
        $token = bin2hex(random_bytes(32));
        $this->session->set('csrf_token', $token);
        return $token;
    }
    
    public function validateCSRFToken($token) {
        $storedToken = $this->session->get('csrf_token');
        
        if (!$storedToken || !hash_equals($storedToken, $token)) {
            return false;
        }
        
        // Generate new token after validation
        $this->generateCSRFToken();
        return true;
    }
    
    private function setRememberMeToken($userId) {
        $token = bin2hex(random_bytes(32));
        $selector = bin2hex(random_bytes(16));
        $hashedToken = hash('sha256', $token);
        
        // Store in database
        $stmt = $this->db->prepare("
            INSERT INTO remember_tokens (selector, hashed_token, user_id, expires_at) 
            VALUES (?, ?, ?, DATE_ADD(NOW(), INTERVAL 30 DAY))
        ");
        $stmt->execute([$selector, $hashedToken, $userId]);
        
        // Set cookie
        $cookieValue = $selector . ':' . $token;
        setcookie('remember_token', $cookieValue, time() + 2592000, '/', '', true, true);
    }
    
    private function validateRememberMeToken($cookieValue) {
        list($selector, $token) = explode(':', $cookieValue);
        
        $stmt = $this->db->prepare("
            SELECT rt.hashed_token, u.id, u.email, u.name, u.role 
            FROM remember_tokens rt 
            JOIN users u ON rt.user_id = u.id 
            WHERE rt.selector = ? AND rt.expires_at > NOW() AND u.is_active = 1
        ");
        $stmt->execute([$selector]);
        $data = $stmt->fetch();
        
        if ($data && hash_equals($data['hashed_token'], hash('sha256', $token))) {
            // Update session
            $this->session->set('user_id', $data['id']);
            $this->session->set('user_email', $data['email']);
            $this->session->set('user_name', $data['name']);
            $this->session->set('user_role', $data['role']);
            $this->session->set('logged_in', true);
            
            // Generate new token
            $newToken = bin2hex(random_bytes(32));
            $newHashedToken = hash('sha256', $newToken);
            
            $updateStmt = $this->db->prepare("
                UPDATE remember_tokens 
                SET hashed_token = ?, expires_at = DATE_ADD(NOW(), INTERVAL 30 DAY) 
                WHERE selector = ?
            ");
            $updateStmt->execute([$newHashedToken, $selector]);
            
            // Update cookie
            $cookieValue = $selector . ':' . $newToken;
            setcookie('remember_token', $cookieValue, time() + 2592000, '/', '', true, true);
            
            return true;
        }
        
        return false;
    }
    
    private function clearRememberMeToken($cookieValue) {
        list($selector, ) = explode(':', $cookieValue);
        $stmt = $this->db->prepare("DELETE FROM remember_tokens WHERE selector = ?");
        $stmt->execute([$selector]);
    }
    
    private function logActivity($userId, $activity) {
        $stmt = $this->db->prepare("
            INSERT INTO user_activity (user_id, activity, ip_address, user_agent, created_at) 
            VALUES (?, ?, ?, ?, NOW())
        ");
        $stmt->execute([
            $userId, 
            $activity, 
            $_SERVER['REMOTE_ADDR'] ?? 'unknown',
            $_SERVER['HTTP_USER_AGENT'] ?? 'unknown'
        ]);
    }
    
    private function sendVerificationEmail($email, $token) {
        // Implement email sending logic here
        $verificationLink = "https://yourdomain.com/verify.php?token=" . $token;
        // Use PHPMailer or similar library
    }
}
?>
