#pragma once

#include <QWidget>

struct CheckBoxPrivate;
class QPropertyAnimation;

class CheckBox : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal checkMarkOffset READ checkMarkOffset WRITE setCheckMarkOffset)

    public:
        explicit CheckBox(QWidget *parent = nullptr);

        void setChecked(bool checked);
        bool checked() const;

        void setDarkMode(bool dark);
        bool darkMode() const;

        void setText(const QString &text);
        QString text() const;

    protected:
        void paintEvent(QPaintEvent *event) override;
        bool event(QEvent *event) override;

    signals:
        void checked(bool checked);
        
    private:
        std::unique_ptr<CheckBoxPrivate> d = nullptr;

        void setCheckMarkOffset(qreal offset);
        qreal checkMarkOffset() const;

        bool hovered() const;
};

struct CheckBoxPrivate {
    friend class CheckBox;

    private:
        bool darkMode = false, checked = false, hovered = false;

        QString text;
        QFontMetrics fm{QFont()};

        const int spacing = 10, checkBoxW = 22, checkBoxH = 22;
        int textW = 0, textH = 0;

        QPropertyAnimation *checkMarkOffsetAnim = nullptr;
        qreal checkMarkOffset = 0.0;
};