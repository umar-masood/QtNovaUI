#pragma once

#include <QScrollBar>
#include <QWidget>

struct ScrollBarPrivate;
class QPropertyAnimation;

class ScrollBar : public QScrollBar {
    Q_OBJECT
    Q_PROPERTY(qreal thumbWidth READ thumbWidth WRITE setThumbWidth)
    Q_PROPERTY(qreal trackOpacity READ trackOpacity WRITE setTrackOpacity)

    public:
        explicit ScrollBar(QWidget *parent = nullptr);

        void setDarkMode(bool dark);
        bool darkMode() const;

    protected:
        bool event(QEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

        bool hovered() const;
        bool pressed() const;

        void setThumbWidth(qreal width);
        qreal thumbWidth() const;

        void setTrackOpacity(qreal opacity);
        qreal trackOpacity() const;

    private:
        std::unique_ptr<ScrollBarPrivate> d = nullptr;
};

// Not for public use
struct ScrollBarPrivate {
    friend class ScrollBar;

    private:
    bool darkMode = false, hovered = false, pressed = false;

    QPropertyAnimation *thumbWidthAnim = nullptr;
    qreal thumbWidth = 3.0;

    QPropertyAnimation *trackOpacityAnim = nullptr;
    qreal trackOpacity = 0.0;
};