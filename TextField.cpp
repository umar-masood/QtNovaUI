#include "TextField.h"

TextField::TextField(const QString &text, QWidget *parent) : QLineEdit(text, parent) { init(); }
TextField::TextField(QWidget *parent) : QLineEdit(parent) { init(); }

void TextField::setShadow(bool enable) { hasShadow = enable; }
void TextField::setDarkMode(bool enable) {
    isDarkMode = enable;

    if (clear) 
        clear->setDarkMode(isDarkMode); 

    if (password) 
        password->setDarkMode(isDarkMode);

    updateStyle();
}

void TextField::setFixedSize(QSize s) {
    int finalWidth = std::max(s.width(), minWidth);
    int finalHeight = s.height();

    QLineEdit::setFixedSize(finalWidth, finalHeight);
}


void TextField::setIconic(bool enable) {
    hasTextFieldIcon = enable;
    updateStyle();
}

void TextField::setIconSize(QSize s) { if (hasTextFieldIcon) textFieldIconSize = s; }

void TextField::setIconPaths(const QString &lightIcon, const QString &darkIcon) {
    if (hasTextFieldIcon) {
        light_icon = lightIcon;
        dark_icon = darkIcon;
    } else {
        light_icon.clear();
        dark_icon.clear();
    }
}

void TextField::setReadOnly(bool enable) {
    isReadOnly = enable;
    QLineEdit::setReadOnly(isReadOnly);
}

void TextField::setEnabled(bool enable) {
    isEnabled = enable;
    QLineEdit::setEnabled(isEnabled);
}

void TextField::setContextMenu(bool enable) { hasContextMenu = enable; }
void TextField::setFontProperties(const QString &family, int pointSize, bool bold, bool italic) {
    isItalic = italic; isBold = bold; fontSize = pointSize; fontFamily = family;
    updateStyle(); 
}

void TextField::setClearButton(bool enable) {
    hasClearButton = enable;

    if (hasClearButton && !clear) {
        clear = new Button(this);
        clear->setCursor(Qt::PointingHandCursor);
        clear->setDisplayMode(Button::IconOnly);
        clear->setNormalBackgroundTransparent(true);
        clear->setBorderTransparent(true);
        clear->setIconSize(this->textFieldIconSize);
        clear->setFixedSize(QSize(28, 28));
        clear->setIconPaths(ClearIcon, ClearIcon);
        clear->setEnabled(isEnabled);
        positionButton(clear);
        
        clear->raise();
        clear->hide();

        connect(clear, &Button::pressed, this, [this]() {
            QTimer::singleShot(250, this, SLOT(clear()));
        });

        connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
            clear->setVisible(isFocused && !text.isEmpty());
        });

        updateStyle();
    }
}

void TextField::setPasswordMode(bool enable) {
    hasPasswordButton = enable;

    if (hasPasswordButton && !password && !hasClearButton) {
        password = new Button(this);
        password->setCursor(Qt::PointingHandCursor);
        password->setNormalBackgroundTransparent(true);
        password->setBorderTransparent(true);
        password->setDisplayMode(Button::IconOnly);
        password->setIconSize(this->textFieldIconSize);
        password->setFixedSize(QSize(28, 28));
        password->setEnabled(isEnabled);
        this->setEchoMode(QLineEdit::Password);
        positionButton(password);

        password->hide();
        password->setIconPaths(HideIcon, HideIcon);

        connect(password, &Button::pressed, this, [this]() {
            isPasswordVisible = !isPasswordVisible;

            if (isPasswordVisible) {
                password->setIconPaths(ShowIcon, ShowIcon);
                this->setEchoMode(QLineEdit::Normal);
            } else {
                password->setIconPaths(HideIcon, HideIcon);
                this->setEchoMode(QLineEdit::Password);
            }
        });

        connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
            password->setVisible(isFocused && !text.isEmpty());

            if (isPasswordVisible) {
                password->setIconPaths(HideIcon, HideIcon);
                this->setEchoMode(QLineEdit::Password);
                isPasswordVisible = false;
            }
        });

        updateStyle();
    }
    update();
}

void TextField::setPadding(int left, int top, int right, int bottom) {
    _left = left; 
    _right = right, 
    _bottom = bottom, 
    _top = top;

    updateStyle();
}

void TextField::resizeEvent(QResizeEvent *event) {
    QLineEdit::resizeEvent(event);
    positionButton(clear);
    positionButton(password);
}

void TextField::positionButton(Button *button) {
    if (button) {
        int x = width() - (12 + button->width()) + 3;
        int y = (height() - button->height()) / 2;
        button->move(x, y);
    }
}

QColor TextField::color(const TextFieldColor &state) const {  return _colors.value(state, Qt::transparent); }
QColor TextField::textColor(const TextFieldTextColor &state) const { return _textColors.value(state, QColor("#000000")); }

void TextField::setColor(const TextFieldColor &state, const QColor &color) {
    _colors.insert(state, color);
    updateStyle();
}

void TextField::setTextColor(const TextFieldTextColor &state, const QColor &color) {
    _textColors.insert(state, color);
    updateStyle();
}

void TextField::setBorderTransparent(bool enable) { isBorderTransparent = enable; }
void TextField::setNormalBackgroundTransparent(bool enable) { isBackgroundTransparent = enable; }

void TextField::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRect rec = rect().adjusted(1, 1, -1, -1);

    // Determine border color
    QColor penColor = isFocused ? color(TextFieldColor::BorderFocused)
                                    : (isDarkMode ? color(TextFieldColor::BorderDark)
                                                  : color(TextFieldColor::BorderLight));
    if (isBorderTransparent) penColor = Qt::transparent;

    QPen pen(penColor, isFocused ? 1.0 : 0.5);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    // Determine background color
    QColor brushColor;
    if (isBackgroundTransparent)
        brushColor = Qt::transparent;
    else if (isFocused)
        brushColor = isDarkMode ? color(TextFieldColor::FocusedDark)
                                : color(TextFieldColor::FocusedLight);
    else if (isHover)
        brushColor = isDarkMode ? color(TextFieldColor::HoverDark)
                                : color(TextFieldColor::HoverLight);
    else
        brushColor = isDarkMode ? color(TextFieldColor::NormalDark)
                                : color(TextFieldColor::NormalLight);
    
    painter.setBrush(brushColor);

    // Draw rounded rect
    QPainterPath path;
    path.addRoundedRect(rec, 6, 6);
    painter.drawPath(path);

    // Draw left icon if present
    if (hasTextFieldIcon) {
        QString iconPath = isDarkMode ? dark_icon : light_icon;
        if (!iconPath.isEmpty()) {
            QPixmap icon = IconManager::renderSvg(iconPath, textFieldIconSize);
            if (!icon.isNull()) {
                int yPos = (rect().height() - icon.height()) / 2;
                int xPos = 12;
                painter.drawPixmap(xPos, yPos, icon);
            }
        }
    }

    // Call base QLineEdit paintEvent to draw text
    QLineEdit::paintEvent(event);
}

void TextField::keyPressEvent(QKeyEvent *event) {
    if ((!hasContextMenu || isReadOnly) && event->modifiers() & Qt::ControlModifier) {
        switch(event->key()) {
            case Qt::Key_C:
            case Qt::Key_V:
            case Qt::Key_X:
            case Qt::Key_A:
                return;

            default: break;
        }
    }

    QLineEdit::keyPressEvent(event);
}

void TextField::enterEvent(QEnterEvent *event) {
    isHover = true;
    QLineEdit::enterEvent(event);
    update();
}

void TextField::leaveEvent(QEvent *event) {
    isHover = false;
    QLineEdit::leaveEvent(event);
    update();
}

void TextField::focusInEvent(QFocusEvent *event) {
    if (!isReadOnly)
        isFocused = true;
    
    if (clear) clear->setVisible(!text().isEmpty());

    if (hasShadow) {
        effect->setColor(QColor("#32CCFE"));
        animate->setStartValue(effect->blurRadius());
        animate->setEndValue(18);
        animate->start();
    }

    QLineEdit::focusInEvent(event);
    update();
}

void TextField::focusOutEvent(QFocusEvent *event) {
    isFocused = false;
 
    if (clear) clear->setVisible(false);

    if (hasShadow) {
        animate->setStartValue(effect->blurRadius());
        animate->setEndValue(0);
        animate->start();
    }

    QLineEdit::focusOutEvent(event);
    update();
}

void TextField::contextMenuEvent(QContextMenuEvent *event) {
    if (!hasContextMenu || !isReadOnly) 
        return;

    menu = new Menu(this);
    menu->setMaxVisibleItems(12);
    menu->setItemSize(QSize(180, 36));
    menu->setDarkMode(isDarkMode);
    menu->setIconic(true);

    const bool hasText = !this->text().isEmpty();
    const bool hasSelection = this->hasSelectedText();

    if (hasSelection) {
        menu->addAction({ "Copy",    false,  "Ctrl + C",  CopyIcon,    CopyIcon });
        menu->addAction({ "Cut",     false,  "Ctrl + X",  CutIcon,     CutIcon });
        menu->addAction({ "Delete",  false,  "Delete",    DeleteIcon,  DeleteIcon });
    }

    menu->addAction({ "Paste", false, "Ctrl + V", PasteIcon, PasteIcon});
    
    if (hasText && !hasSelection) menu->addAction({ "Select All",  false,  "Ctrl + A",  SelectAllIcon,  SelectAllIcon});
    if (this->isUndoAvailable())  menu->addAction({ "Undo",        false,  "Ctrl + Z",  UndoIcon,       UndoIcon});
    if (this->isRedoAvailable())  menu->addAction({ "Redo",        false,  "Ctrl + Y",  RedoIcon,       RedoIcon});

    connect(menu, &Menu::itemClicked, this, [=]() {
        QString action = menu->clickedItemText();
        if (action == "Copy") copy();
        else if (action == "Cut") cut();
        else if (action == "Paste") paste();
        else if (action == "Delete") del();
        else if (action == "Select All") selectAll();
        else if (action == "Undo") undo();
        else if (action == "Redo") redo();
        else qDebug() << "Unknown context menu action:" << action;
    });

    if (menu->hasActions())
        menu->show();
    menu->move(event->globalPos());
}

void TextField::init() {
    setFixedSize(QSize(0, 0));
    setFocusPolicy(Qt::ClickFocus);
    loadDefaultColors();
    updateStyle();
    
    effect = new SmoothShadow(this);
    effect->setOffset(0, 0);
    effect->setColor(QColor(0, 0, 0, 0));
    effect->setBlurRadius(0);

    if (hasShadow)
        setGraphicsEffect(effect);

    animate = new QPropertyAnimation(effect, "blurRadius", this);
    animate->setDuration(300);
    animate->setEasingCurve(QEasingCurve::InOutQuad);
}

void TextField::loadDefaultColors() {
    // Border colors
    _colors[TextFieldColor::BorderFocused] = QColor("#0191DF");      // focused border
    _colors[TextFieldColor::BorderLight]   = QColor("#CCCCCC");      // light mode normal border
    _colors[TextFieldColor::BorderDark]    = QColor("#4D4D4D");      // dark mode normal border

    // Background colors
    _colors[TextFieldColor::FocusedLight]  = QColor("#FFFFFF");      // focused background light
    _colors[TextFieldColor::FocusedDark]   = QColor("#242424");      // focused background dark
    _colors[TextFieldColor::HoverLight]    = QColor("#F0F0F0");      // hover background light
    _colors[TextFieldColor::HoverDark]     = QColor("#323232");      // hover background dark
    _colors[TextFieldColor::NormalLight]   = QColor("#FBFBFB");      // normal background light
    _colors[TextFieldColor::NormalDark]    = QColor("#2D2D2D");      // normal background dark

    // Text colors
    _textColors[TextFieldTextColor::NormalTxtLight]         = QColor("#000000"); // normal text light mode
    _textColors[TextFieldTextColor::NormalTxtDark]          = QColor("#FFFFFF");  // normal text dark mode
    _textColors[TextFieldTextColor::SelectedTxt]            = QColor("#FFFFFF");  // selected text
    _textColors[TextFieldTextColor::PlaceHolderTxt]         = QColor("#ACABAB");  // placeholder text
    _textColors[TextFieldTextColor::SelectionBackgroundTxt] = QColor("#32CCFE");  // selection background
}

void TextField::updateStyle() {
    QString styleSheet = QString(R"(
      QLineEdit {
        font-family: '%1';
        font-size: %2pt;
        font-weight: %3;
        font-style: %4;
        background-color: transparent;
        border-radius: 0px;
        border: none;
        outline: none;
        color: %5;
        selection-background-color: %6;
        selection-color: %7;
        placeholder-text-color: %8;
        padding-left: %9px;
        padding-right: %10px;
        padding-bottom: %11px;
        padding-top: %12px;
      })")
        .arg(fontFamily)
        .arg(fontSize)
        .arg(isBold ? "bold" : "normal")
        .arg(isItalic ? "italic" : "normal")
        .arg(textColor(isDarkMode ? NormalTxtDark : NormalTxtLight).name())             
        .arg(textColor(SelectionBackgroundTxt).name())
        .arg(textColor(SelectedTxt).name())        
        .arg(textColor(PlaceHolderTxt).name())      
        .arg(_left != 0 ? _left : (hasTextFieldIcon ? (12 + 20 + 12) - 3 : 12))
        .arg(_right != 0 ? _right : ((hasClearButton || hasPasswordButton) ? (24 + 28) - 9 : 12))
        .arg(_bottom != 0 ? _bottom : 2)
        .arg(_top != 0 ? _top : 0);

    setStyleSheet(styleSheet);
}
