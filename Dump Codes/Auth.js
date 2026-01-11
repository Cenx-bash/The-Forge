class AuthManager {
    constructor() {
        this.csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute('content');
        this.initEventListeners();
    }
    
    initEventListeners() {
        // Login form
        const loginForm = document.getElementById('loginForm');
        if (loginForm) {
            loginForm.addEventListener('submit', (e) => this.handleLogin(e));
        }
        
        // Register form
        const registerForm = document.getElementById('registerForm');
        if (registerForm) {
            registerForm.addEventListener('submit', (e) => this.handleRegister(e));
        }
        
        // Password visibility toggle
        document.querySelectorAll('.toggle-password').forEach(button => {
            button.addEventListener('click', (e) => this.togglePasswordVisibility(e));
        });
        
        // Logout
        const logoutBtn = document.getElementById('logoutBtn');
        if (logoutBtn) {
            logoutBtn.addEventListener('click', (e) => this.handleLogout(e));
        }
    }
    
    async handleLogin(e) {
        e.preventDefault();
        
        const form = e.target;
        const formData = new FormData(form);
        const submitBtn = form.querySelector('button[type="submit"]');
        const originalText = submitBtn.textContent;
        
        // Show loading state
        submitBtn.disabled = true;
        submitBtn.innerHTML = '<i class="spinner-border spinner-border-sm"></i> Logging in...';
        
        // Clear previous errors
        this.clearErrors(form);
        
        try {
            const response = await fetch('api/login.php', {
                method: 'POST',
                headers: {
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                },
                body: formData
            });
            
            const result = await response.json();
            
            if (result.success) {
                if (result.requires_2fa) {
                    // Redirect to 2FA page
                    window.location.href = 'two-factor.php';
                } else {
                    // Show success message
                    this.showToast('success', 'Login successful! Redirecting...');
                    
                    // Redirect after delay
                    setTimeout(() => {
                        window.location.href = result.redirect || 'dashboard.php';
                    }, 1500);
                }
            } else {
                // Show error
                this.showFieldError(form, 'email', result.message);
                this.showToast('error', result.message);
            }
            
        } catch (error) {
            console.error('Login error:', error);
            this.showToast('error', 'Network error. Please try again.');
        } finally {
            // Reset button
            submitBtn.disabled = false;
            submitBtn.textContent = originalText;
        }
    }
    
    async handleRegister(e) {
        e.preventDefault();
        
        const form = e.target;
        const formData = new FormData(form);
        const submitBtn = form.querySelector('button[type="submit"]');
        const originalText = submitBtn.textContent;
        
        // Validate passwords match
        const password = form.querySelector('#password').value;
        const confirmPassword = form.querySelector('#confirm_password').value;
        
        if (password !== confirmPassword) {
            this.showFieldError(form, 'confirm_password', 'Passwords do not match');
            return;
        }
        
        // Show loading state
        submitBtn.disabled = true;
        submitBtn.innerHTML = '<i class="spinner-border spinner-border-sm"></i> Creating account...';
        
        // Clear previous errors
        this.clearErrors(form);
        
        try {
            const response = await fetch('api/register.php', {
                method: 'POST',
                headers: {
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                },
                body: formData
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.showToast('success', 'Registration successful!');
                
                // Redirect after delay
                setTimeout(() => {
                    window.location.href = 'dashboard.php';
                }, 2000);
            } else {
                // Show field-specific errors
                if (result.errors) {
                    Object.keys(result.errors).forEach(field => {
                        this.showFieldError(form, field, result.errors[field]);
                    });
                } else {
                    this.showFieldError(form, 'email', result.message);
                }
                this.showToast('error', result.message);
            }
            
        } catch (error) {
            console.error('Registration error:', error);
            this.showToast('error', 'Network error. Please try again.');
        } finally {
            // Reset button
            submitBtn.disabled = false;
            submitBtn.textContent = originalText;
        }
    }
    
    async handleLogout(e) {
        e.preventDefault();
        
        if (!confirm('Are you sure you want to logout?')) {
            return;
        }
        
        try {
            const response = await fetch('api/logout.php', {
                method: 'POST',
                headers: {
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                }
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.showToast('success', 'Logged out successfully');
                setTimeout(() => {
                    window.location.href = 'index.php';
                }, 1000);
            }
            
        } catch (error) {
            console.error('Logout error:', error);
            this.showToast('error', 'Logout failed');
        }
    }
    
    showFieldError(form, fieldName, message) {
        const field = form.querySelector(`[name="${fieldName}"]`);
        if (!field) return;
        
        // Add error class
        field.classList.add('is-invalid');
        
        // Show error message
        let errorElement = field.nextElementSibling;
        if (!errorElement || !errorElement.classList.contains('invalid-feedback')) {
            errorElement = document.createElement('div');
            errorElement.className = 'invalid-feedback';
            field.parentNode.insertBefore(errorElement, field.nextSibling);
        }
        errorElement.textContent = message;
    }
    
    clearErrors(form) {
        form.querySelectorAll('.is-invalid').forEach(el => {
            el.classList.remove('is-invalid');
        });
        
        form.querySelectorAll('.invalid-feedback').forEach(el => {
            el.remove();
        });
    }
    
    showToast(type, message) {
        // Create toast element
        const toastId = 'toast-' + Date.now();
        const toastHtml = `
            <div id="${toastId}" class="toast align-items-center text-bg-${type} border-0" role="alert">
                <div class="d-flex">
                    <div class="toast-body">
                        ${message}
                    </div>
                    <button type="button" class="btn-close btn-close-white me-2 m-auto" data-bs-dismiss="toast"></button>
                </div>
            </div>
        `;
        
        // Add to toast container
        const container = document.querySelector('.toast-container') || this.createToastContainer();
        container.insertAdjacentHTML('beforeend', toastHtml);
        
        // Show toast
        const toastElement = document.getElementById(toastId);
        const toast = new bootstrap.Toast(toastElement, { delay: 3000 });
        toast.show();
        
        // Remove after hide
        toastElement.addEventListener('hidden.bs.toast', () => {
            toastElement.remove();
        });
    }
    
    createToastContainer() {
        const container = document.createElement('div');
        container.className = 'toast-container position-fixed top-0 end-0 p-3';
        document.body.appendChild(container);
        return container;
    }
    
    togglePasswordVisibility(e) {
        const button = e.target.closest('.toggle-password');
        const input = button.previousElementSibling;
        const icon = button.querySelector('i');
        
        if (input.type === 'password') {
            input.type = 'text';
            icon.classList.remove('bi-eye');
            icon.classList.add('bi-eye-slash');
        } else {
            input.type = 'password';
            icon.classList.remove('bi-eye-slash');
            icon.classList.add('bi-eye');
        }
    }
    
    // Password strength checker
    checkPasswordStrength(password) {
        let strength = 0;
        const feedback = [];
        
        if (password.length >= 8) strength++;
        else feedback.push('At least 8 characters');
        
        if (/[a-z]/.test(password)) strength++;
        else feedback.push('Lowercase letters');
        
        if (/[A-Z]/.test(password)) strength++;
        else feedback.push('Uppercase letters');
        
        if (/[0-9]/.test(password)) strength++;
        else feedback.push('Numbers');
        
        if (/[^A-Za-z0-9]/.test(password)) strength++;
        else feedback.push('Special characters');
        
        return {
            score: strength,
            max: 5,
            feedback: feedback
        };
    }
}

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.authManager = new AuthManager();
});
