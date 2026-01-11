class CartManager {
    constructor() {
        this.csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute('content');
        this.initEventListeners();
        this.updateCartCount();
    }
    
    initEventListeners() {
        // Add to cart buttons
        document.addEventListener('click', (e) => {
            const addToCartBtn = e.target.closest('.add-to-cart');
            if (addToCartBtn) {
                e.preventDefault();
                this.addToCart(addToCartBtn);
            }
        });
        
        // Cart quantity updates
        document.addEventListener('click', (e) => {
            const incrementBtn = e.target.closest('.increment-quantity');
            const decrementBtn = e.target.closest('.decrement-quantity');
            const removeBtn = e.target.closest('.remove-item');
            
            if (incrementBtn) {
                e.preventDefault();
                this.updateQuantity(incrementBtn, 1);
            }
            
            if (decrementBtn) {
                e.preventDefault();
                this.updateQuantity(decrementBtn, -1);
            }
            
            if (removeBtn) {
                e.preventDefault();
                this.removeItem(removeBtn);
            }
        });
        
        // Cart page quantity inputs
        document.addEventListener('change', (e) => {
            if (e.target.classList.contains('cart-quantity-input')) {
                this.updateQuantityInput(e.target);
            }
        });
        
        // Clear cart button
        const clearCartBtn = document.getElementById('clearCartBtn');
        if (clearCartBtn) {
            clearCartBtn.addEventListener('click', (e) => this.clearCart(e));
        }
    }
    
    async addToCart(button) {
        const productId = button.dataset.productId;
        const quantity = button.dataset.quantity || 1;
        
        // Get product attributes if any
        const attributes = this.getProductAttributes(button);
        
        // Show loading state
        const originalHtml = button.innerHTML;
        button.disabled = true;
        button.innerHTML = '<i class="spinner-border spinner-border-sm"></i> Adding...';
        
        try {
            const response = await fetch('api/cart.php?action=add', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                },
                body: JSON.stringify({
                    product_id: productId,
                    quantity: parseInt(quantity),
                    attributes: attributes
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.showSuccessMessage('Item added to cart!');
                this.updateCartCount(result.cart_count);
                
                // Update button to show success
                button.innerHTML = '<i class="bi bi-check-lg"></i> Added!';
                setTimeout(() => {
                    button.innerHTML = originalHtml;
                    button.disabled = false;
                }, 1500);
            } else {
                this.showErrorMessage(result.message);
                button.innerHTML = originalHtml;
                button.disabled = false;
            }
            
        } catch (error) {
            console.error('Add to cart error:', error);
            this.showErrorMessage('Failed to add item to cart');
            button.innerHTML = originalHtml;
            button.disabled = false;
        }
    }
    
    async updateQuantity(button, change) {
        const itemId = button.dataset.itemId || button.closest('[data-item-id]')?.dataset.itemId;
        const productId = button.dataset.productId || button.closest('[data-product-id]')?.dataset.productId;
        
        if (!itemId && !productId) return;
        
        const currentQuantity = parseInt(button.closest('.quantity-container')?.querySelector('.quantity')?.textContent || 1);
        const newQuantity = currentQuantity + change;
        
        if (newQuantity < 1) {
            this.removeItem(button);
            return;
        }
        
        try {
            const response = await fetch('api/cart.php?action=update', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                },
                body: JSON.stringify({
                    item_id: itemId,
                    product_id: productId,
                    quantity: newQuantity
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.updateCartDisplay(result);
            } else {
                this.showErrorMessage(result.message);
            }
            
        } catch (error) {
            console.error('Update quantity error:', error);
            this.showErrorMessage('Failed to update quantity');
        }
    }
    
    async updateQuantityInput(input) {
        const itemId = input.dataset.itemId;
        const quantity = parseInt(input.value);
        
        if (quantity < 1) {
            input.value = 1;
            return;
        }
        
        try {
            const response = await fetch('api/cart.php?action=update', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                },
                body: JSON.stringify({
                    item_id: itemId,
                    quantity: quantity
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.updateCartDisplay(result);
            } else {
                this.showErrorMessage(result.message);
                // Revert to previous value
                input.value = quantity - 1;
            }
            
        } catch (error) {
            console.error('Update quantity error:', error);
            this.showErrorMessage('Failed to update quantity');
        }
    }
    
    async removeItem(button) {
        if (!confirm('Remove item from cart?')) {
            return;
        }
        
        const itemId = button.dataset.itemId || button.closest('[data-item-id]')?.dataset.itemId;
        const productId = button.dataset.productId || button.closest('[data-product-id]')?.dataset.productId;
        
        if (!itemId && !productId) return;
        
        try {
            const response = await fetch('api/cart.php?action=remove', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                },
                body: JSON.stringify({
                    item_id: itemId,
                    product_id: productId
                })
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.showSuccessMessage('Item removed from cart');
                this.updateCartCount(result.cart_count);
                
                // Remove item from DOM
                const itemElement = button.closest('.cart-item, tr[data-item-id]');
                if (itemElement) {
                    itemElement.remove();
                    this.updateCartTotals(result);
                }
                
                // If cart is empty, show empty cart message
                if (result.cart_count === 0) {
                    this.showEmptyCart();
                }
            } else {
                this.showErrorMessage(result.message);
            }
            
        } catch (error) {
            console.error('Remove item error:', error);
            this.showErrorMessage('Failed to remove item');
        }
    }
    
    async clearCart(e) {
        e.preventDefault();
        
        if (!confirm('Clear all items from cart?')) {
            return;
        }
        
        try {
            const response = await fetch('api/cart.php?action=clear', {
                method: 'POST',
                headers: {
                    'X-Requested-With': 'XMLHttpRequest',
                    'X-CSRF-Token': this.csrfToken
                }
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.showSuccessMessage('Cart cleared successfully');
                this.updateCartCount(0);
                this.showEmptyCart();
            } else {
                this.showErrorMessage(result.message);
            }
            
        } catch (error) {
            console.error('Clear cart error:', error);
            this.showErrorMessage('Failed to clear cart');
        }
    }
    
    updateCartDisplay(result) {
        // Update cart count
        if (result.cart_count !== undefined) {
            this.updateCartCount(result.cart_count);
        }
        
        // Update totals
        this.updateCartTotals(result);
        
        // Update specific item if needed
        if (result.item_total !== undefined && result.product_id) {
            const itemElement = document.querySelector(`[data-product-id="${result.product_id}"]`);
            if (itemElement) {
                const totalElement = itemElement.querySelector('.item-total');
                if (totalElement) {
                    totalElement.textContent = this.formatCurrency(result.item_total);
                }
            }
        }
    }
    
    updateCartTotals(result) {
        // Update subtotal
        const subtotalElement = document.getElementById('cartSubtotal');
        if (subtotalElement && result.cart_total !== undefined) {
            subtotalElement.textContent = this.formatCurrency(result.cart_total);
        }
        
        // Update tax and total if applicable
        const taxElement = document.getElementById('cartTax');
        const totalElement = document.getElementById('cartTotal');
        
        if (totalElement && result.cart_total !== undefined) {
            const tax = result.cart_total * 0.08; // 8% tax example
            const total = result.cart_total + tax;
            
            if (taxElement) {
                taxElement.textContent = this.formatCurrency(tax);
            }
            totalElement.textContent = this.formatCurrency(total);
        }
    }
    
    updateCartCount(count) {
        // Update all cart count elements
        document.querySelectorAll('.cart-count').forEach(element => {
            element.textContent = count || 0;
            element.style.display = count > 0 ? 'inline' : 'none';
        });
        
        // Update cart icon
        const cartIcon = document.querySelector('.cart-icon');
        if (cartIcon) {
            if (count > 0) {
                cartIcon.classList.add('has-items');
            } else {
                cartIcon.classList.remove('has-items');
            }
        }
    }
    
    showEmptyCart() {
        const cartContainer = document.querySelector('.cart-items-container, tbody');
        if (cartContainer) {
            cartContainer.innerHTML = `
                <tr>
                    <td colspan="6" class="text-center py-5">
                        <i class="bi bi-cart-x display-1 text-muted"></i>
                        <h4 class="mt-3">Your cart is empty</h4>
                        <p class="text-muted">Add some items to get started!</p>
                        <a href="products.php" class="btn btn-primary mt-2">Continue Shopping</a>
                    </td>
                </tr>
            `;
        }
        
        // Hide checkout button
        const checkoutBtn = document.querySelector('.checkout-btn');
        if (checkoutBtn) {
            checkoutBtn.style.display = 'none';
        }
    }
    
    getProductAttributes(button) {
        const attributes = {};
        const form = button.closest('form');
        
        if (form) {
            const selects = form.querySelectorAll('select[name^="attribute_"]');
            selects.forEach(select => {
                const name = select.name.replace('attribute_', '');
                attributes[name] = select.value;
            });
        }
        
        return Object.keys(attributes).length > 0 ? attributes : null;
    }
    
    showSuccessMessage(message) {
        this.showMessage(message, 'success');
    }
    
    showErrorMessage(message) {
        this.showMessage(message, 'error');
    }
    
    showMessage(message, type) {
        // Create notification
        const notification = document.createElement('div');
        notification.className = `alert alert-${type === 'error' ? 'danger' : 'success'} alert-dismissible fade show position-fixed`;
        notification.style.cssText = 'top: 20px; right: 20px; z-index: 1050; min-width: 300px;';
        notification.innerHTML = `
            ${message}
            <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
        `;
        
        // Add to document
        document.body.appendChild(notification);
        
        // Auto remove after 3 seconds
        setTimeout(() => {
            notification.remove();
        }, 3000);
    }
    
    formatCurrency(amount) {
        return new Intl.NumberFormat('en-US', {
            style: 'currency',
            currency: 'USD'
        }).format(amount);
    }
}

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.cartManager = new CartManager();
});
