<?php
require_once __DIR__ . '/../config/database.php';
require_once __DIR__ . '/SessionManager.php';

class Cart {
    private $db;
    private $session;
    private $cartId;
    
    public function __construct() {
        $this->db = Database::getInstance();
        $this->session = new SessionManager();
        $this->initializeCart();
    }
    
    private function initializeCart() {
        if (!$this->session->get('cart_id')) {
            $cartId = uniqid('cart_', true);
            $this->session->set('cart_id', $cartId);
        }
        $this->cartId = $this->session->get('cart_id');
    }
    
    public function addItem($productId, $quantity = 1, $attributes = []) {
        // Validate quantity
        if ($quantity <= 0) {
            return ['success' => false, 'message' => 'Invalid quantity'];
        }
        
        // Check product availability
        $product = $this->getProductDetails($productId);
        if (!$product) {
            return ['success' => false, 'message' => 'Product not found'];
        }
        
        if ($product['stock_quantity'] < $quantity) {
            return ['success' => false, 'message' => 'Insufficient stock'];
        }
        
        // Check if item already exists in cart
        $existingItem = $this->getCartItem($productId, $attributes);
        
        if ($existingItem) {
            $newQuantity = $existingItem['quantity'] + $quantity;
            
            if ($product['stock_quantity'] < $newQuantity) {
                return ['success' => false, 'message' => 'Cannot add more items. Stock limit reached.'];
            }
            
            return $this->updateItemQuantity($productId, $newQuantity, $attributes);
        }
        
        // Add new item
        try {
            $stmt = $this->db->prepare("
                INSERT INTO cart_items (cart_id, product_id, quantity, attributes, added_at) 
                VALUES (?, ?, ?, ?, NOW())
            ");
            
            $attributesJson = !empty($attributes) ? json_encode($attributes) : null;
            $stmt->execute([$this->cartId, $productId, $quantity, $attributesJson]);
            
            return [
                'success' => true, 
                'message' => 'Item added to cart',
                'cart_count' => $this->getCartCount()
            ];
            
        } catch (PDOException $e) {
            error_log("Cart add error: " . $e->getMessage());
            return ['success' => false, 'message' => 'Failed to add item to cart'];
        }
    }
    
    public function updateItemQuantity($productId, $quantity, $attributes = []) {
        if ($quantity <= 0) {
            return $this->removeItem($productId, $attributes);
        }
        
        // Check stock
        $product = $this->getProductDetails($productId);
        if ($product['stock_quantity'] < $quantity) {
            return ['success' => false, 'message' => 'Insufficient stock'];
        }
        
        $itemId = $this->getCartItemId($productId, $attributes);
        
        if (!$itemId) {
            return ['success' => false, 'message' => 'Item not found in cart'];
        }
        
        try {
            $stmt = $this->db->prepare("
                UPDATE cart_items 
                SET quantity = ?, updated_at = NOW() 
                WHERE id = ?
            ");
            $stmt->execute([$quantity, $itemId]);
            
            return [
                'success' => true, 
                'message' => 'Cart updated',
                'cart_total' => $this->getCartTotal(),
                'item_total' => $product['price'] * $quantity
            ];
            
        } catch (PDOException $e) {
            error_log("Cart update error: " . $e->getMessage());
            return ['success' => false, 'message' => 'Failed to update cart'];
        }
    }
    
    public function removeItem($productId, $attributes = []) {
        $itemId = $this->getCartItemId($productId, $attributes);
        
        if (!$itemId) {
            return ['success' => false, 'message' => 'Item not found in cart'];
        }
        
        try {
            $stmt = $this->db->prepare("DELETE FROM cart_items WHERE id = ?");
            $stmt->execute([$itemId]);
            
            return [
                'success' => true, 
                'message' => 'Item removed from cart',
                'cart_count' => $this->getCartCount(),
                'cart_total' => $this->getCartTotal()
            ];
            
        } catch (PDOException $e) {
            error_log("Cart remove error: " . $e->getMessage());
            return ['success' => false, 'message' => 'Failed to remove item'];
        }
    }
    
    public function getCartItems() {
        $stmt = $this->db->prepare("
            SELECT ci.*, p.name, p.price, p.sku, p.stock_quantity, p.image_url 
            FROM cart_items ci 
            JOIN products p ON ci.product_id = p.id 
            WHERE ci.cart_id = ? AND p.is_active = 1
            ORDER BY ci.added_at DESC
        ");
        $stmt->execute([$this->cartId]);
        $items = $stmt->fetchAll();
        
        // Calculate totals
        foreach ($items as &$item) {
            $item['total'] = $item['price'] * $item['quantity'];
            $item['attributes'] = $item['attributes'] ? json_decode($item['attributes'], true) : [];
        }
        
        return $items;
    }
    
    public function getCartCount() {
        $stmt = $this->db->prepare("
            SELECT SUM(quantity) as total_items 
            FROM cart_items 
            WHERE cart_id = ?
        ");
        $stmt->execute([$this->cartId]);
        $result = $stmt->fetch();
        
        return $result['total_items'] ?? 0;
    }
    
    public function getCartTotal() {
        $items = $this->getCartItems();
        $total = 0;
        
        foreach ($items as $item) {
            $total += $item['price'] * $item['quantity'];
        }
        
        return $total;
    }
    
    public function clearCart() {
        try {
            $stmt = $this->db->prepare("DELETE FROM cart_items WHERE cart_id = ?");
            $stmt->execute([$this->cartId]);
            
            // Generate new cart ID
            $newCartId = uniqid('cart_', true);
            $this->session->set('cart_id', $newCartId);
            $this->cartId = $newCartId;
            
            return ['success' => true, 'message' => 'Cart cleared'];
            
        } catch (PDOException $e) {
            error_log("Clear cart error: " . $e->getMessage());
            return ['success' => false, 'message' => 'Failed to clear cart'];
        }
    }
    
    public function mergeWithUserCart($userId) {
        if (!$userId) return false;
        
        try {
            // Get user's saved cart
            $stmt = $this->db->prepare("
                SELECT cart_id FROM user_carts 
                WHERE user_id = ? 
                ORDER BY updated_at DESC 
                LIMIT 1
            ");
            $stmt->execute([$userId]);
            $userCart = $stmt->fetch();
            
            if ($userCart) {
                // Merge carts
                $this->db->prepare("
                    UPDATE cart_items 
                    SET cart_id = ? 
                    WHERE cart_id = ?
                ")->execute([$userCart['cart_id'], $this->cartId]);
                
                $this->session->set('cart_id', $userCart['cart_id']);
                $this->cartId = $userCart['cart_id'];
            } else {
                // Save current cart for user
                $this->saveCartForUser($userId);
            }
            
            return true;
            
        } catch (PDOException $e) {
            error_log("Cart merge error: " . $e->getMessage());
            return false;
        }
    }
    
    private function saveCartForUser($userId) {
        try {
            $stmt = $this->db->prepare("
                INSERT INTO user_carts (user_id, cart_id, updated_at) 
                VALUES (?, ?, NOW())
                ON DUPLICATE KEY UPDATE cart_id = ?, updated_at = NOW()
            ");
            $stmt->execute([$userId, $this->cartId, $this->cartId]);
            
        } catch (PDOException $e) {
            error_log("Save cart error: " . $e->getMessage());
        }
    }
    
    private function getProductDetails($productId) {
        $stmt = $this->db->prepare("
            SELECT id, name, price, stock_quantity 
            FROM products 
            WHERE id = ? AND is_active = 1
        ");
        $stmt->execute([$productId]);
        return $stmt->fetch();
    }
    
    private function getCartItem($productId, $attributes = []) {
        $attributesJson = !empty($attributes) ? json_encode($attributes) : null;
        
        $stmt = $this->db->prepare("
            SELECT * FROM cart_items 
            WHERE cart_id = ? AND product_id = ? 
            AND (attributes = ? OR (attributes IS NULL AND ? IS NULL))
        ");
        $stmt->execute([$this->cartId, $productId, $attributesJson, $attributesJson]);
        
        return $stmt->fetch();
    }
    
    private function getCartItemId($productId, $attributes = []) {
        $item = $this->getCartItem($productId, $attributes);
        return $item ? $item['id'] : null;
    }
}
?>
