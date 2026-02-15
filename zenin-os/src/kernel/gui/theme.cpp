#include <gui/theme.hpp>
#include <gui/window.hpp>
#include <lib/vector.hpp>
#include <memory/memory.hpp>

namespace Zenin {
namespace GUI {

class TokyoNightTheme : public Theme {
private:
    struct ColorPalette {
        Color background = {0x1A, 0x1B, 0x26};    // #1A1B26
        Color foreground = {0xA9, 0xB1, 0xD6};    // #A9B1D6
        Color blue = {0x7A, 0xA2, 0xF7};          // #7AA2F7
        Color purple = {0xBB, 0x9A, 0xF7};        // #BB9AF7
        Color cyan = {0x7D, 0xCF, 0xFF};          // #7DCFFF
        Color green = {0x9E, 0xCE, 0x6A};         // #9ECE6A
        Color yellow = {0xE0, 0xAF, 0x68};        // #E0AF68
        Color red = {0xF7, 0x76, 0x8E};           // #F7768E
    };
    
    ColorPalette palette;
    
public:
    TokyoNightTheme() {
        name = "Tokyo Night";
        version = "1.0";
        author = "Zenin OS Team";
    }
    
    void apply(Window* window) override {
        window->setBackgroundColor(palette.background);
        window->setForegroundColor(palette.foreground);
        window->setBorderColor(palette.blue);
        window->setTitleColor(palette.cyan);
        
        // Apply Tokyo Night specific styling
        applyWindowEffects(window);
    }
    
    void drawWindowDecoration(Window* window, Framebuffer* fb) override {
        Rect bounds = window->getBounds();
        
        // Draw Tokyo Night styled title bar
        fb->drawRect(bounds.x, bounds.y, bounds.width, 30, palette.blue);
        
        // Draw title with gradient effect
        drawTextWithShadow(fb, 
                          window->getTitle().c_str(),
                          bounds.x + 10, 
                          bounds.y + 8,
                          palette.foreground,
                          palette.background);
        
        // Draw Tokyo Night styled window controls
        drawWindowControls(fb, bounds);
    }
    
    void drawButton(Button* button, Framebuffer* fb) override {
        Rect bounds = button->getBounds();
        Color baseColor = button->isPressed() ? darken(palette.blue, 20) : palette.blue;
        
        // Draw button with Tokyo Night gradient
        fb->drawGradientRect(bounds.x, bounds.y, 
                            bounds.width, bounds.height,
                            baseColor,
                            lighten(baseColor, 15));
        
        // Add subtle shadow
        fb->drawRect(bounds.x + 2, bounds.y + 2, 
                    bounds.width, bounds.height,
                    {0, 0, 0, 50});
    }
    
private:
    Color darken(Color c, uint8_t amount) {
        return {
            static_cast<uint8_t>(c.r * (100 - amount) / 100),
            static_cast<uint8_t>(c.g * (100 - amount) / 100),
            static_cast<uint8_t>(c.b * (100 - amount) / 100),
            c.a
        };
    }
    
    Color lighten(Color c, uint8_t amount) {
        return {
            static_cast<uint8_t>(c.r + (255 - c.r) * amount / 100),
            static_cast<uint8_t>(c.g + (255 - c.g) * amount / 100),
            static_cast<uint8_t>(c.b + (255 - c.b) * amount / 100),
            c.a
        };
    }
    
    void drawWindowControls(Framebuffer* fb, Rect bounds) {
        // Close button (red)
        fb->drawCircle(bounds.x + bounds.width - 20, 
                      bounds.y + 15, 6, palette.red);
        
        // Minimize button (yellow)
        fb->drawCircle(bounds.x + bounds.width - 40, 
                      bounds.y + 15, 6, palette.yellow);
        
        // Maximize button (green)
        fb->drawCircle(bounds.x + bounds.width - 60, 
                      bounds.y + 15, 6, palette.green);
    }
    
    void drawTextWithShadow(Framebuffer* fb, const char* text, 
                           int x, int y, Color fg, Color bg) {
        fb->drawText(text, x + 1, y + 1, bg);
        fb->drawText(text, x, y, fg);
    }
    
    void applyWindowEffects(Window* window) {
        // Add Tokyo Night specific window effects
        window->setOpacity(0.95f);
        window->setShadowEnabled(true);
        window->setShadowColor({0, 0, 0, 100});
        window->setCornerRadius(8);
    }
};

// Theme manager singleton
class ThemeManager {
private:
    Vector<Theme*> themes;
    Theme* currentTheme;
    
public:
    static ThemeManager& instance() {
        static ThemeManager instance;
        return instance;
    }
    
    void registerTheme(Theme* theme) {
        themes.push_back(theme);
    }
    
    void setTheme(const String& name) {
        for (auto theme : themes) {
            if (theme->getName() == name) {
                currentTheme = theme;
                notifyThemeChange();
                return;
            }
        }
    }
    
    Theme* getCurrentTheme() {
        return currentTheme;
    }
    
private:
    void notifyThemeChange() {
        // Notify all windows of theme change
        WindowManager::instance().applyThemeToAllWindows(currentTheme);
    }
};

} // namespace GUI
} // namespace Zenin