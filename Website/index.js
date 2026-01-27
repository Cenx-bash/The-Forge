// ========== MAIN APPLICATION SCRIPT ==========

document.addEventListener('DOMContentLoaded', function() {
    // Initialize components
    initMobileMenu();
    initPerformanceChart();
    initSmoothScroll();
    initNavbarScroll();
    initServiceCardAnimations();
    initPortfolioHoverEffects();
    
    console.log('Premium website initialized successfully!');
});

// ========== MOBILE MENU FUNCTIONALITY ==========
function initMobileMenu() {
    const menuBtn = document.querySelector('.mobile-menu-btn');
    const closeBtn = document.querySelector('.mobile-menu-close');
    const mobileMenu = document.querySelector('.mobile-menu-overlay');
    const mobileLinks = document.querySelectorAll('.mobile-nav-link');
    
    // Open mobile menu
    menuBtn.addEventListener('click', () => {
        mobileMenu.classList.add('active');
        document.body.style.overflow = 'hidden';
    });
    
    // Close mobile menu
    closeBtn.addEventListener('click', closeMobileMenu);
    
    // Close when clicking overlay
    mobileMenu.addEventListener('click', (e) => {
        if (e.target === mobileMenu) {
            closeMobileMenu();
        }
    });
    
    // Close when clicking links
    mobileLinks.forEach(link => {
        link.addEventListener('click', closeMobileMenu);
    });
    
    // Close on escape key
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape') {
            closeMobileMenu();
        }
    });
    
    function closeMobileMenu() {
        mobileMenu.classList.remove('active');
        document.body.style.overflow = 'auto';
    }
}

// ========== PERFORMANCE CHART ==========
function initPerformanceChart() {
    const ctx = document.getElementById('performanceChart');
    
    if (!ctx) return;
    
    // Generate performance data
    const months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun'];
    const performanceData = {
        labels: months,
        datasets: [{
            label: 'Performance',
            data: [65, 75, 80, 88, 92, 95],
            borderColor: '#6366f1',
            backgroundColor: 'rgba(99, 102, 241, 0.1)',
            borderWidth: 3,
            fill: true,
            tension: 0.4,
            pointBackgroundColor: '#6366f1',
            pointBorderColor: '#ffffff',
            pointBorderWidth: 2,
            pointRadius: 6
        }]
    };
    
    const chart = new Chart(ctx, {
        type: 'line',
        data: performanceData,
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: false
                },
                tooltip: {
                    backgroundColor: '#0f172a',
                    titleColor: '#ffffff',
                    bodyColor: '#e2e8f0',
                    borderColor: '#6366f1',
                    borderWidth: 1,
                    cornerRadius: 8,
                    padding: 12,
                    displayColors: false
                }
            },
            scales: {
                x: {
                    grid: {
                        display: false
                    },
                    ticks: {
                        color: '#64748b'
                    }
                },
                y: {
                    min: 50,
                    max: 100,
                    grid: {
                        color: 'rgba(226, 232, 240, 0.5)'
                    },
                    ticks: {
                        color: '#64748b',
                        callback: function(value) {
                            return value + '%';
                        }
                    }
                }
            },
            interaction: {
                intersect: false,
                mode: 'index'
            }
        }
    });
    
    return chart;
}

// ========== SMOOTH SCROLL ==========
function initSmoothScroll() {
    const links = document.querySelectorAll('a[href^="#"]');
    
    links.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            
            const targetId = this.getAttribute('href');
            if (targetId === '#') return;
            
            const targetElement = document.querySelector(targetId);
            if (targetElement) {
                const headerHeight = document.querySelector('.navbar').offsetHeight;
                const targetPosition = targetElement.offsetTop - headerHeight - 20;
                
                window.scrollTo({
                    top: targetPosition,
                    behavior: 'smooth'
                });
            }
        });
    });
}

// ========== NAVBAR SCROLL EFFECT ==========
function initNavbarScroll() {
    const navbar = document.querySelector('.navbar');
    let lastScroll = 0;
    
    window.addEventListener('scroll', () => {
        const currentScroll = window.pageYOffset;
        
        // Add shadow on scroll
        if (currentScroll > 50) {
            navbar.style.boxShadow = '0 4px 20px rgba(0, 0, 0, 0.08)';
            navbar.style.padding = '15px 0';
        } else {
            navbar.style.boxShadow = 'none';
            navbar.style.padding = '20px 0';
        }
        
        // Hide/show on scroll direction
        if (currentScroll > lastScroll && currentScroll > 100) {
            navbar.style.transform = 'translateY(-100%)';
        } else {
            navbar.style.transform = 'translateY(0)';
        }
        
        lastScroll = currentScroll;
    });
}

// ========== SERVICE CARD ANIMATIONS ==========
function initServiceCardAnimations() {
    const serviceCards = document.querySelectorAll('.service-card');
    
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.style.opacity = '1';
                entry.target.style.transform = 'translateY(0)';
            }
        });
    }, {
        threshold: 0.1
    });
    
    serviceCards.forEach((card, index) => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(30px)';
        card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        card.style.transitionDelay = (index * 0.1) + 's';
        
        observer.observe(card);
    });
}

// ========== PORTFOLIO HOVER EFFECTS ==========
function initPortfolioHoverEffects() {
    const portfolioItems = document.querySelectorAll('.portfolio-item');
    
    portfolioItems.forEach(item => {
        const image = item.querySelector('.portfolio-image');
        const info = item.querySelector('.portfolio-info');
        
        item.addEventListener('mouseenter', () => {
            image.style.transform = 'scale(1.05)';
            info.style.transform = 'translateY(-10px)';
        });
        
        item.addEventListener('mouseleave', () => {
            image.style.transform = 'scale(1)';
            info.style.transform = 'translateY(0)';
        });
    });
}

// ========== FORM VALIDATION (For Future Forms) ==========
function validateForm(form) {
    const inputs = form.querySelectorAll('input[required], textarea[required]');
    let isValid = true;
    
    inputs.forEach(input => {
        if (!input.value.trim()) {
            input.style.borderColor = '#ef4444';
            isValid = false;
        } else {
            input.style.borderColor = '#10b981';
        }
    });
    
    return isValid;
}

// ========== THEME TOGGLE (Future Enhancement) ==========
function initThemeToggle() {
    const themeToggle = document.createElement('button');
    themeToggle.className = 'theme-toggle';
    themeToggle.innerHTML = '<i class="fas fa-moon"></i>';
    themeToggle.style.position = 'fixed';
    themeToggle.style.bottom = '20px';
    themeToggle.style.right = '20px';
    themeToggle.style.zIndex = '1000';
    themeToggle.style.width = '50px';
    themeToggle.style.height = '50px';
    themeToggle.style.borderRadius = '50%';
    themeToggle.style.background = 'var(--gradient)';
    themeToggle.style.color = 'white';
    themeToggle.style.border = 'none';
    themeToggle.style.cursor = 'pointer';
    themeToggle.style.boxShadow = 'var(--shadow-lg)';
    
    document.body.appendChild(themeToggle);
    
    themeToggle.addEventListener('click', () => {
        document.body.classList.toggle('dark-theme');
        themeToggle.innerHTML = document.body.classList.contains('dark-theme') 
            ? '<i class="fas fa-sun"></i>' 
            : '<i class="fas fa-moon"></i>';
    });
}

// ========== LOADING ANIMATION ==========
function showLoadingAnimation() {
    const loading = document.createElement('div');
    loading.className = 'loading-animation';
    loading.innerHTML = `
        <div class="loading-spinner">
            <div class="spinner"></div>
            <p>Loading Premium Experience...</p>
        </div>
    `;
    
    loading.style.position = 'fixed';
    loading.style.top = '0';
    loading.style.left = '0';
    loading.style.width = '100%';
    loading.style.height = '100%';
    loading.style.background = 'var(--dark)';
    loading.style.display = 'flex';
    loading.style.alignItems = 'center';
    loading.style.justifyContent = 'center';
    loading.style.zIndex = '9999';
    loading.style.color = 'white';
    loading.style.fontSize = '18px';
    
    document.body.appendChild(loading);
    
    // Remove after 1.5 seconds (simulating load time)
    setTimeout(() => {
        loading.style.opacity = '0';
        loading.style.transition = 'opacity 0.5s ease';
        setTimeout(() => {
            document.body.removeChild(loading);
        }, 500);
    }, 1500);
}

// ========== PERFORMANCE METRICS ==========
function trackPerformance() {
    // Track page load time
    window.addEventListener('load', () => {
        const timing = performance.timing;
        const loadTime = timing.loadEventEnd - timing.navigationStart;
        console.log(`Page loaded in ${loadTime}ms`);
        
        // Send to analytics (placeholder)
        if (loadTime < 3000) {
            console.log('Excellent performance!');
        }
    });
    
    // Track user interactions
    document.addEventListener('click', (e) => {
        const target = e.target;
        if (target.classList.contains('btn-primary')) {
            console.log('Primary button clicked:', target.textContent);
        }
    });
}

// Initialize performance tracking
trackPerformance();

// Show loading animation on initial load
window.addEventListener('load', () => {
    // Remove initial loader after everything is ready
    const initialLoader = document.querySelector('.loading-animation');
    if (initialLoader) {
        setTimeout(() => {
            initialLoader.style.opacity = '0';
            setTimeout(() => initialLoader.remove(), 500);
        }, 1000);
    }
});

// ========== ADDITIONAL ENHANCEMENTS ==========

// Parallax effect on hero section
function initParallaxEffect() {
    const hero = document.querySelector('.hero');
    
    window.addEventListener('scroll', () => {
        const scrolled = window.pageYOffset;
        const rate = scrolled * -0.5;
        
        if (hero) {
            hero.style.transform = `translate3d(0, ${rate}px, 0)`;
        }
    });
}

// Initialize parallax if hero exists
if (document.querySelector('.hero')) {
    initParallaxEffect();
}

// Tooltip system
function initTooltips() {
    const elements = document.querySelectorAll('[data-tooltip]');
    
    elements.forEach(el => {
        el.addEventListener('mouseenter', (e) => {
            const tooltip = document.createElement('div');
            tooltip.className = 'tooltip';
            tooltip.textContent = e.target.dataset.tooltip;
            document.body.appendChild(tooltip);
            
            const rect = e.target.getBoundingClientRect();
            tooltip.style.position = 'fixed';
            tooltip.style.left = rect.left + rect.width / 2 + 'px';
            tooltip.style.top = rect.top - 40 + 'px';
            tooltip.style.transform = 'translateX(-50%)';
            tooltip.style.background = 'var(--dark)';
            tooltip.style.color = 'white';
            tooltip.style.padding = '8px 12px';
            tooltip.style.borderRadius = '6px';
            tooltip.style.fontSize = '14px';
            tooltip.style.zIndex = '10000';
            
            e.target._tooltip = tooltip;
        });
        
        el.addEventListener('mouseleave', (e) => {
            if (e.target._tooltip) {
                document.body.removeChild(e.target._tooltip);
                e.target._tooltip = null;
            }
        });
    });
}

// Initialize tooltips
initTooltips();

// Export functions for module usage (if needed)
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        initMobileMenu,
        initPerformanceChart,
        initSmoothScroll,
        validateForm
    };
}
