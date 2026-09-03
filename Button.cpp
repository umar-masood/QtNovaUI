#include "Button.h"

#include <QFile>
#include <QEvent>
#include <QPainter>

Button::Button(QWidget *parent) : QPushButton(parent), 
                                  d(std::make_unique<ButtonPrivate>()) {
    setFont(font());
}

Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent), 
                                                       d(std::make_unique<ButtonPrivate>()) {
    setFont(font());
}

void Button::setThemeIcon(const QString &light, const QString &dark) {
    if (d->style == Button::Style::TextOnly) {
        qWarning() << "Button style is not set to Iconic.";
        return;
    }

    if (light.isEmpty() || dark.isEmpty()) {
        qWarning() << "One of theme icon paths is empty.";
        return;
    }

    if (!QFile::exists(light)) {
        qWarning() << "Light theme icon path does not exist.";
        return;
    }

    if (!QFile::exists(dark)) {
        qWarning() << "Dark theme icon path does not exist.";
        return;
    }

    d->darkIcon = QPixmap(dark).scaled(iconSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    d->lightIcon = QPixmap(light).scaled(iconSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);;

    update();
}

void Button::setFont(const QFont &font) {
    QPushButton::setFont(font);
    d->fm = QFontMetrics(font);

    update();
}

void Button::setCornerRadius(int radius) {
    if (d->cornerRadius == radius)
        return;
    
    d->cornerRadius = radius;

    update();
}

int Button::cornerRadius() const {
    return d->cornerRadius;
}

void Button::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool Button::darkMode() const {
    return d->darkMode;
}

void Button::setStyle(Button::Style style) {
    if (d->style == style)
        return;

    d->style = style;

    update();
}

Button::Style Button::style() const {
    return d->style;
}

void Button::setVariant(Button::Variant variant) {
    if (d->variant == variant)
        return;
    
    d->variant = variant;

    update();
}

Button::Variant Button::variant() const {
    return d->variant;
}

void Button::setLoading(bool loading) {
    if (d->loading == loading)
        return;
    
    d->loading = loading;

    update();
}

bool Button::loading() const {
    return d->loading;
}

bool Button::pressed() const {
    return d->pressed;
}

bool Button::hovered() const {
    return d->hovered;
}

bool Button::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            d->pressed = true;
            update();
            break;
        
        case QEvent::MouseButtonRelease:
            d->pressed = false;
            update();
            break;

        case QEvent::Enter:
            d->hovered = true;
            update();
            break;

        case QEvent::Leave:
            d->hovered = false;
            update();
            break;

        case QEvent::Hide:
            d->pressed = false;
            d->hovered = false;
            update();
            break;
    }

    return QPushButton::event(event);
}

void Button::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRect rec(rect().adjusted(1, 1, -1, -1));

    // Drawing border
    painter.setPen(borderPen());

    // Drawing background
    painter.setBrush(brush());
    painter.drawRoundedRect(rec, cornerRadius(), cornerRadius());

    // Drawing Text & Icon 
    painter.setPen(textPen());    
    painter.setOpacity(pressed() ? 0.6 : 1.0);

    const int textW = d->fm.horizontalAdvance(text());
    const int textH = d->fm.height();

    const int spacing = 8;

    switch (variant()) {
        case Button::Variant::Primary:
        case Button::Variant::Secondary:
        case Button::Variant::Ghost: {
            switch (style()) {
                case Button::Style::IconOnly: {
                    const int iconX = (width() - iconSize().width()) / 2;
                    const int iconY = (height() - iconSize().height()) / 2;
                    painter.drawPixmap(iconX, iconY, darkMode() ? d->darkIcon : d->lightIcon);
                }
                break;

                case Button::Style::TextOnly: {
                    const int textX = (width() - textW) / 2;
                    const int textY = (height() - textH) / 2 + d->fm.ascent();
                    painter.drawText(textX, textY, text());
                }
                break;

                case Button::Style::IconText: {
                    const int contentW = 3 * spacing + iconSize().width() + textW;
                    const int contentH = height();
                    const int contentX = (width() - contentW) / 2;
                    const int contentY = (height() - contentH) / 2;

                    const QRect contentRect(contentX, contentY, contentW, contentH);

                    const int iconX = contentRect.left() + spacing;
                    const int iconY = contentRect.top() + (contentRect.height() - iconSize().height()) / 2;
                    painter.drawPixmap(iconX, iconY, darkMode() ? d->darkIcon : d->lightIcon);

                    const int textX = contentRect.left() + 2 * spacing + iconSize().width();
                    const int textY = contentRect.top() + (contentRect.height() - textH) / 2 + d->fm.ascent();
                    painter.drawText(textX, textY, text());
                }
                break;

                case Button::Style::TextUnderIcon: {                    
                    const int contentW = 2 * spacing + qMax(textW, iconSize().width());
                    const int contentH = 3 * spacing + iconSize().height() + textH;
                    const int contentX = (width() - contentW) / 2;
                    const int contentY = (height() - contentH) / 2;

                    const QRect contentRect(contentX, contentY, contentW, contentH);

                    const int iconX = contentRect.left() + (contentRect.width() - iconSize().width()) / 2;
                    const int iconY = contentRect.top() + spacing;
                    painter.drawPixmap(iconX, iconY, darkMode() ? d->darkIcon : d->lightIcon);

                    const int textX = contentRect.left() + (contentRect.width() - textW) / 2;
                    const int textY = iconY + iconSize().height() + spacing + d->fm.ascent();
                    painter.drawText(textX, textY, text());
                }
                break;
            }
        }
        break;

        case Button::Variant::Link: {
            switch (style()) {
                case Button::Style::IconOnly:
                case Button::Style::TextUnderIcon:
                    qWarning() << "IconOnly or TextUnderIcon styles are not allowed for Link Button.";
                break;

                case Button::Style::TextOnly: {
                    const int textX = (width() - textW) / 2;
                    const int textY = (height() - textH) / 2 + d->fm.ascent();
                    painter.drawText(textX, textY, text());
                }
                break;

                case Button::Style::IconText: {
                    const int contentW = textW + spacing + iconSize().width();
                    const int contentH = height();
                    const int contentX = (width() - contentW) / 2;
                    const int contentY = (height() - contentH) / 2;

                    const QRect contentRect(contentX, contentY, contentW, contentH);

                    const int textX = contentRect.left();
                    const int textY = contentRect.top() + (contentRect.height() - textH) / 2 + d->fm.ascent();
                    painter.drawText(textX, textY, text());

                    const int iconX = textX + textW + spacing;
                    const int iconY = contentRect.top() + (contentRect.height() - iconSize().height()) / 2;
                    painter.drawPixmap(iconX, iconY, darkMode() ? d->darkIcon : d->lightIcon);
                }
                break;
            }
        }
        break;
    }
}

// Colors
QPen Button::textPen() const {
    switch (variant()) {
        case Button::Variant::Primary: 
            return QPen(QColor("#FFFFFF"));
        
        case Button::Variant::Ghost: 
        case Button::Variant::Secondary: {
            if (!isEnabled())
                return QPen(darkMode() ? "#777777" : "#9E9E9E");
            else 
                return QPen(darkMode() ? "#F0F0F0" : "#000000");
        }

        case Button::Variant::Link: {
            if (!isEnabled())
                return QPen(darkMode() ? "#1E3A8A" : "#93C5FD");
            else if (pressed())
                return QPen(darkMode() ? "#2563EB" : "#174EA6");
            else if (hovered())
                return QPen(darkMode() ? "#3B82F6" : "#1D5FD1");
            else 
                return QPen(darkMode() ? "#60A5FA" : "#2563EB");
        }
    }

    return Qt::NoPen;
}

QPen Button::borderPen() const {
    switch (variant()) {
        case Button::Variant::Primary:
        case Button::Variant::Ghost:
        case Button::Variant::Link:
            return Qt::NoPen;

        default:
            return QPen(darkMode() ? QColor(255, 255, 255, 30) : QColor(0, 0, 0, 30), 0.6);
    }
}

QBrush Button::brush() const {
    QColor color;
    switch (variant()) {
        case Button::Variant::Primary: {
            if (!isEnabled())
                color = darkMode() ? "#1E3A8A" : "#93C5FD";
            else if (pressed())
                color = darkMode() ? "#1D4ED8" : "#1E40AF";
            else if (hovered())
                color = darkMode() ? "#2563EB" : "#1D4ED8";
            else
                color = darkMode() ? "#3B82F6" : "#2563EB";
        }
        break;
    
        case Button::Variant::Secondary: {
            if (!isEnabled())
                color = darkMode() ? "#1F1F1F" : "#E5E7EB";
            else if (pressed())
                color = darkMode() ? "#242424" : "#EAEAEA";
            else if (hovered())
                color = darkMode() ? "#363636" : "#F5F5F5";
            else 
                color = darkMode() ? "#2D2D2D" : "#FFFFFF";
        }
        break;

        case Button::Variant::Link:
            return Qt::NoBrush;

        case Button::Variant::Ghost: {
            if (!isEnabled())
                color = QColor(0, 0, 0, 0);
            else if (pressed())
                color = darkMode() ? QColor(255, 255, 255, 31) : QColor(0, 0, 0, 26);
            else if (hovered())
                color = darkMode() ? QColor(255, 255, 255, 20)  : QColor(0, 0, 0, 15);
            else
                color = Qt::transparent;

        }
        break;
    }

    return QBrush(color);
}
