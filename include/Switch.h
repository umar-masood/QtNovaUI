#pragma once

#include <QPushButton>

struct SwitchPrivate;
class QPropertyAnimation;

class Switch : public QWidget {
    Q_OBJECT

    Q_PROPERTY(qreal knobScale READ knobScale WRITE setKnobScale)
    Q_PROPERTY(qreal knobOffset READ knobOffset WRITE setKnobOffset)
    Q_PROPERTY(qreal knobWidth READ knobWidth WRITE setKnobWidth)

    public:
        explicit Switch(QWidget *parent = nullptr);
        
        void setChecked(bool checked);
        bool checked() const;

        void setDarkMode(bool dark);
        bool darkMode() const;

    protected:
        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;

    private:
        std::unique_ptr<SwitchPrivate> d = nullptr;

        bool hovered() const;

        // For Animations
        void setKnobWidth(int width);
        int knobWidth() const;

        void setKnobScale(qreal scale);
        qreal knobScale() const;

        void setKnobOffset(qreal offset);
        qreal knobOffset() const;
        
    signals:
        void toggled(bool toggled);
};

struct SwitchPrivate {
    friend class Switch;

    private:
        bool darkMode = false, checked = false, hovered = false;
        const int width = 38, height = 22;

        QPropertyAnimation *knobScaleAnim = nullptr;
        qreal knobScale = 1.0;

        QPropertyAnimation *knobOffsetAnim = nullptr;
        qreal knobOffset = 0.0;

        QPropertyAnimation *knobWidthAnim = nullptr;
        int knobWidth = 14;
};