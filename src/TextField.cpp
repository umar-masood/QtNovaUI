#include "../include/TextField.h"
#include "../include/Button.h"
#include "../resources/Icons.h"

#include <QFile>
#include <QPixmap>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QPalette>

TextField::TextField(QWidget *parent) : QLineEdit(parent), d(std::make_unique<TextFieldPrivate>()) {  
    setStyleSheet(
        "QLineEdit {"
        "border: none;"
        "background: transparent;"
        "}"
    );  

    // Helper
    auto makeButton = [this](){
        auto *button = new Button(this);
        button->setCursor(Qt::PointingHandCursor);
        button->setFocusPolicy(Qt::NoFocus);
        button->setVariant(Button::Variant::Ghost);
        button->setStyle(Button::Style::IconOnly);
        button->setIconSize(iconSize());
        button->setFixedSize(d->actionButtonSize);
        button->raise();
        button->hide();
        
        return button;
    };

    // Clear Button
    d->clearButton = makeButton();
    d->clearButton->setThemeIcon(icon(Icons::ClearLight), icon(Icons::ClearDark));

    connect(d->clearButton, &Button::clicked, this, &QLineEdit::clear);

    connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
        d->clearButton->setVisible(d->clearButtonEnabled && focused() && !text.isEmpty());
    });

    // Password Button
    d->passwordButton = makeButton();
    d->passwordButton->setThemeIcon(icon(Icons::HideLight), icon(Icons::HideDark));

    connect(d->passwordButton, &Button::clicked, this, [this]() {
        d->passwordVisible = !d->passwordVisible;

        if (d->passwordVisible) {
            d->passwordButton->setThemeIcon(icon(Icons::ShowLight), icon(Icons::ShowDark));
            setEchoMode(QLineEdit::Normal);
        } else {
            d->passwordButton->setThemeIcon(icon(Icons::HideLight), icon(Icons::HideDark));
            setEchoMode(QLineEdit::Password);
        }
    });

    connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
        d->passwordButton->setVisible(d->passwordButtonEnabled && focused() && !text.isEmpty());

        if (d->passwordVisible) {
            d->passwordButton->setThemeIcon(icon(Icons::HideLight), icon(Icons::HideDark));
            setEchoMode(QLineEdit::Password);
            d->passwordVisible = false;
        }
    });

    // Margins
    adjustTextMargins();

    // Palette
    updatePalette();
}

void TextField::adjustTextMargins() {
    int leftMargin = d->spacing;
    int rightMargin = d->spacing;

    if (d->textFieldIcon)
        leftMargin += iconSize().width() + d->spacing;

    if (clearButtonEnabled() || passwordButtonEnabled())
        rightMargin += d->actionButtonSize.width() + d->spacing;

    setTextMargins(leftMargin, 0, rightMargin, 0);
}

void TextField::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    d->clearButton->setDarkMode(d->darkMode);
    d->passwordButton->setDarkMode(d->darkMode);

    update();
    updatePalette();
}

bool TextField::darkMode() const {
    return d->darkMode;
}

void TextField::setIconSize(const QSize &size) {
    if (d->iconSize == size)
        return;

    d->iconSize = size;

    d->clearButton->setIconSize(size);
    d->passwordButton->setIconSize(size);

    adjustTextMargins();
    update();
}

QSize TextField::iconSize() const {
    return d->iconSize;
}

void TextField::setThemeIcon(const QString &light, const QString &dark) {
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

    d->darkIcon = renderSVGIcon(dark, iconSize());
    d->lightIcon = renderSVGIcon(light, iconSize());
    d->textFieldIcon = !d->textFieldIcon;

    update();
    adjustTextMargins();
}

void TextField::setClearButtonEnabled(bool enable) {
    if (d->clearButtonEnabled == enable)
        return;

    d->clearButtonEnabled = enable;

    if (d->clearButtonEnabled) {
        positionButton(d->clearButton);
        d->clearButton->setVisible(focused() && !text().isEmpty());

        d->passwordButtonEnabled = false; // When clear button is enabled then no password button will show
        d->passwordButton->hide();
    } else {
        d->clearButton->hide();
    }

    adjustTextMargins();
}

bool TextField::clearButtonEnabled() const {
    return d->clearButtonEnabled;
}

void TextField::setPasswordButtonEnabled(bool enable) {
    if (d->passwordButtonEnabled == enable)
        return;

    d->passwordButtonEnabled = enable;

    if (d->passwordButtonEnabled) {
        positionButton(d->passwordButton);
        d->passwordButton->setVisible(focused() && !text().isEmpty());

        d->clearButtonEnabled = false; // When password button is enabled then no clear button will show
        d->clearButton->hide();
    } else {
        d->passwordButton->hide();
    }

    adjustTextMargins();
}

bool TextField::passwordButtonEnabled() const {
    return d->passwordButtonEnabled;
}

void TextField::positionButton(Button *button) {
    if (!button)
        return;

    QRect rec = rect().adjusted(1, 1, -1, -1);
    int x = rect().right() - button->width() - d->spacing;
    int y = rec.top() + (rec.height() - button->height()) / 2;

    button->move(x, y);
}

void TextField::updatePalette() {
    QPalette pal = palette();
    pal.setColor(QPalette::Text, darkMode() ? QColor("#FFFFFF") : QColor("#000000"));
    pal.setColor(QPalette::PlaceholderText, QColor("#ACABAB"));
    pal.setColor(QPalette::Highlight, QColor("#32CCFE"));
    pal.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    pal.setColor(QPalette::Base, Qt::transparent);
    setPalette(pal);
}

bool TextField::hovered() const {
    return d->hovered;
}

bool TextField::focused() const {
    return d->focused;
}

void TextField::setContextMenuEnabled(bool enable) {
    if (d->contextMenuEnabled == enable)
        return;

    d->contextMenuEnabled = enable;

    update();
}

bool TextField::contextMenuEnabled() const {
    return d->contextMenuEnabled;
}

void TextField::contextMenuEvent(QContextMenuEvent *event) {
    if (!contextMenuEnabled())
        return;

    QLineEdit::contextMenuEvent(event);
}

bool TextField::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::Enter:
            d->hovered = true;
            update();
        break;

        case QEvent::Leave:
            d->hovered = false;
            update();
        break;

        case QEvent::Hide:
            d->hovered = false;
            d->focused = false;
            update();
        break;

        case QEvent::FocusIn: {
            if (!isReadOnly())
                d->focused = true;

            d->clearButton->setVisible(d->clearButtonEnabled && !text().isEmpty());
            d->passwordButton->setVisible(d->passwordButtonEnabled && !text().isEmpty());

            update();
        }
        break;

        case QEvent::FocusOut: {
            d->focused = false;

            d->clearButton->hide();
            d->passwordButton->hide();

            update();
        }
        break;

        case QEvent::KeyPress: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (!contextMenuEnabled() && (keyEvent->modifiers() & Qt::ControlModifier)) {
                switch (keyEvent->key()) {
                    case Qt::Key_C:
                    case Qt::Key_V:
                    case Qt::Key_X:
                    case Qt::Key_A:
                        return true;

                    default:
                        break;
                }
            }
        }
        break;

        case QEvent::Resize:
            positionButton(d->clearButton);
            positionButton(d->passwordButton);
        break;
        
    }

    return QLineEdit::event(event);
}

void TextField::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRect rec = rect().adjusted(1, 1, -1, -1);

    // Border Color
    QColor penColor = focused() ? QColor("#0191DF") : (darkMode() ? QColor("#4D4D4D") : QColor("#CCCCCC"));
    painter.setPen(QPen(penColor, focused() ? 1.0 : 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // Background Color
    QColor brushColor;
    if (focused())
        brushColor = darkMode() ? QColor("#242424") : QColor("#FFFFFF");
    else if (hovered())
        brushColor = darkMode() ? QColor("#323232") : QColor("#F0F0F0");
    else
        brushColor = darkMode() ? QColor("#2D2D2D") : QColor("#FBFBFB");

    painter.setBrush(brushColor);
    painter.drawRoundedRect(rec, 6, 6);

    // Draw left icon if present
    if (d->textFieldIcon) {
        int iconX = d->spacing;
        int iconY = (rec.height() - iconSize().height()) / 2;
        painter.drawPixmap(iconX, iconY, darkMode() ? d->darkIcon : d->lightIcon);
    }

    QLineEdit::paintEvent(event);
}
