#pragma once 

#include <QPushButton>
#include <memory>

struct ButtonPrivate;

class Button : public QPushButton {
    Q_OBJECT

    public:
        explicit Button(QWidget *parent = nullptr);
        explicit Button(const QString &text, QWidget *parent = nullptr);

        enum class Style {
            IconOnly,
            TextOnly,
            IconText,
            TextUnderIcon
        };

        enum class Variant {
            Primary,
            Secondary,
            Ghost,
            Link
        };

        void setThemeIcon(const QString &light, const QString &dark);
        void setFont(const QFont &font);
        
        void setCornerRadius(int radius);
        int cornerRadius() const;
    
        void setDarkMode(bool dark);
        bool darkMode() const;

        void setStyle(Style style);
        Style style() const;

        void setVariant(Variant variant);
        Variant variant() const;

        void setLoading(bool loading);
        bool loading() const;
    
        bool pressed() const;
        bool hovered() const;

    protected:
        void paintEvent(QPaintEvent *event) override;
        bool event(QEvent *event) override;

    private:
        std::unique_ptr<ButtonPrivate> d = nullptr;

        QPen textPen() const;
        QPen borderPen() const;
        QBrush brush() const;

        void init();
};

// Not for public API
class SpinnerProgress;
struct ButtonPrivate {
    friend class Button;

    private:
        QPixmap lightIcon, darkIcon;
        int cornerRadius = 6, angle = 0;

        bool darkMode = false, loading = false, hovered = false, pressed = false;

        Button::Variant variant = Button::Variant::Secondary;
        Button::Style style = Button::Style::TextOnly;

        QFontMetrics fm{QFont()};
        QTimer *timer = nullptr;
};