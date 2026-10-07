#include "../include/ScrollBar.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>

ScrollBar::ScrollBar(QWidget *parent) : QScrollBar(parent), 
                                        d(std::make_unique<ScrollBarPrivate>()) {

    d->thumbWidthAnim = new QPropertyAnimation(this, "thumbWidth", this);
    d->thumbWidthAnim->setDuration(300);
    d->thumbWidthAnim->setEasingCurve(QEasingCurve::InOutQuad);

    d->trackOpacityAnim = new QPropertyAnimation(this, "trackOpacity", this);
    d->trackOpacityAnim->setDuration(300);
    d->trackOpacityAnim->setEasingCurve(QEasingCurve::InOutQuad);
}

void ScrollBar::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool ScrollBar::darkMode() const {
    return d->darkMode;
}

bool ScrollBar::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::Enter: {
            d->hovered = true;
            
            d->trackOpacityAnim->stop();
            d->trackOpacityAnim->setStartValue(0.0);
            d->trackOpacityAnim->setEndValue(1.0);
            d->trackOpacityAnim->start();

            d->thumbWidthAnim->stop();
            d->thumbWidthAnim->setStartValue(3.0);
            d->thumbWidthAnim->setEndValue(6.0);
            d->thumbWidthAnim->start();

            update();
        }
        break;

        case QEvent::Leave: {
            d->hovered = false;

            d->thumbWidthAnim->stop();
            d->thumbWidthAnim->setStartValue(6.0);
            d->thumbWidthAnim->setEndValue(3.0);
            d->thumbWidthAnim->start();

            d->trackOpacityAnim->stop();
            d->trackOpacityAnim->setStartValue(1.0);
            d->trackOpacityAnim->setEndValue(0.0);
            d->trackOpacityAnim->start();

            update();
        }
        break;

        case QEvent::MouseButtonPress: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) 
                d->pressed = true;
            update();
        }
        break;

        case QEvent::MouseButtonRelease: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) 
                d->pressed = false;
            update();
        }
        break;
    };

    return QScrollBar::event(event);
}

bool ScrollBar::hovered() const {
    return d->hovered;
}

bool ScrollBar::pressed() const {
    return d->pressed;
}

void ScrollBar::setThumbWidth(qreal width) {
    if (d->thumbWidth == width)
        return;

    d->thumbWidth = width;

    update();
}

qreal ScrollBar::thumbWidth() const {
    return d->thumbWidth;
}

void ScrollBar::setTrackOpacity(qreal opacity) {
    if (d->trackOpacity == opacity)
        return;

    d->trackOpacity = opacity;

    update();
}

qreal ScrollBar::trackOpacity() const {
    return d->trackOpacity;
}

void ScrollBar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QRect rec = rect();
    const int radius = 4;

    // Track
    QRect trackRect = rect();
    QColor trackColor = darkMode() ? QColor("#383838") :  QColor("#F0F0F0");
    painter.setPen(Qt::NoPen);

    painter.save();
    painter.setBrush(trackColor);

    if (orientation() == Qt::Vertical) {
        trackRect.setWidth(8.0);
        trackRect.moveCenter(rect().center());
    } else {
        trackRect.setHeight(8.0);
        trackRect.moveCenter(rect().center());
    }

    painter.setOpacity(d->trackOpacity);
    painter.drawRoundedRect(trackRect, radius, radius);
    painter.restore();

    // Thumb
    QColor thumbColor = darkMode() ? QColor("#CCCCCC") : QColor("#707070"); 
    painter.setPen(Qt::NoPen);
    painter.setBrush(thumbColor);

    qreal thumbRectX = 0, thumbRectY = 0, thumbRectW = 0, thumbRectH = 0;
    int range = maximum() - minimum(); // Let's say, if total content height is 1000px and pageStep is 200px, then maximum = 800, minimum = 0;
    if (range < 0)
        return;

    int totalContentH = pageStep() + range;

    if (orientation() == Qt::Vertical) {
        const qreal thumbMaxH = ((double)pageStep() / totalContentH) * rec.height();

        thumbRectH = std::max(20.0, thumbMaxH);
        thumbRectW = d->thumbWidth; 
        thumbRectX = rec.left() + (rec.width() - thumbRectW) / 2.0;
        thumbRectY = (height() - thumbRectH) * (double)value() / range;
    } else {
        const qreal thumbMaxW = ((double)pageStep() / totalContentH) * rec.width();

        qreal &thumbHeight = d->thumbWidth;
        thumbRectH = thumbHeight;
        thumbRectW = std::max(20.0, thumbMaxW);
        thumbRectX = (width() - thumbRectW) * (double)value() / range;
        thumbRectY = rec.top() + (rec.height() - thumbRectH) / 2.0;
    }

    QRectF thumbRect(thumbRectX, thumbRectY, thumbRectW, thumbRectH);
    painter.drawRoundedRect(thumbRect, radius, radius);
}