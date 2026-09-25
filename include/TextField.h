#pragma once

#include <QLineEdit>

struct TextFieldPrivate;
class QPixmap;
class QSize;
class Button;

class TextField : public QLineEdit {
    Q_OBJECT

    public:
        explicit TextField(QWidget *parent = nullptr);

        void setDarkMode(bool dark);
        bool darkMode() const;

        void setIconSize(const QSize &size);
        QSize iconSize() const;

        void setThemeIcon(const QString &light, const QString &dark);
        
        void setClearButtonEnabled(bool enable);
        bool clearButtonEnabled() const;

        void setPasswordButtonEnabled(bool enable);
        bool passwordButtonEnabled() const;

        void setContextMenuEnabled(bool enable);
        bool contextMenuEnabled() const;

    protected:
        void paintEvent(QPaintEvent *event) override;
        void contextMenuEvent(QContextMenuEvent *event) override;
        bool event(QEvent *event) override;

    private:
        std::unique_ptr<TextFieldPrivate> d = nullptr;

        bool hovered() const;
        bool focused() const;

        void positionButton(Button *button);
        void adjustTextMargins();
        void updatePalette();
};

struct TextFieldPrivate {
    friend class TextField;

    private:
        bool darkMode = false, contextMenuEnabled = true, focused = false, hovered = false, textFieldIcon = false, clearButtonEnabled = false, passwordButtonEnabled = false, passwordVisible = false;

        const int spacing = 12;

        QPixmap lightIcon, darkIcon;
        QSize iconSize = QSize(20, 20);

        Button *clearButton = nullptr;
        Button *passwordButton = nullptr;
        const QSize actionButtonSize = QSize(28, 28);
};