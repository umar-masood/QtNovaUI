#include "../include/SpinnerProgress.h"
#include "../SmoothOpacity.h"

#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QPen>

SpinnerProgress::SpinnerProgress(QWidget *parent) : QWidget(parent), 
                                                    d(std::make_unique<SpinnerProgressPrivate>()) {
    setAttribute(Qt::WA_TranslucentBackground);
    hide();
                                                        
    // Opacity
    d->opacity = new SmoothOpacity;
    setGraphicsEffect(d->opacity);
                                                        
    // Anim
    d->animation = new QPropertyAnimation(d->opacity, "opacity");
    d->animation->setDuration(300);
    d->animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Rings Timer
    d->timer = new QTimer(this);
    d->timer->setInterval(32);
    d->timer->setTimerType(Qt::PreciseTimer);
    connect(d->timer, &QTimer::timeout, this, [this](){
        d->outerAngle = (d->outerAngle + 6) % 360;

        if (this->indeterminate())
            d->innerAngle = (d->innerAngle + 8) % 360;

        update();
    });
}

void SpinnerProgress::start() {
    if (!d->timer->isActive())
        d->timer->start(); 

    d->started = true;

    fadeIn();
}

bool SpinnerProgress::started() const {
    return d->started;
}

void SpinnerProgress::stop() {
    d->started = false;

    if (d->timer && d->timer->isActive()) 
        d->timer->stop();

    fadeOut();
}

// void SpinnerProgress::setDarkMode(bool dark) {
//     if (d->darkMode == dark)
//         return;
    
//     d->darkMode = dark;

//     update();
// }

// bool SpinnerProgress::darkMode() const {
//     return d->darkMode;
// }

void SpinnerProgress::setIndeterminate(bool indeterminate) {
    if (d->indeterminate == indeterminate)
        return;

    d->indeterminate = indeterminate;
}

bool SpinnerProgress::indeterminate() const {
    return d->indeterminate;
}

void SpinnerProgress::setRange(int min, int max) {
    if (min < 0 || max > 100 || min >= max) {
        d->min = 0; d->max = 100;
    } else {
        d->min = min; d->max = max;
    }
}

void SpinnerProgress::setValue(int value) {
    if (value < d->min || value > d->max) {
        qWarning() << "Your provided value is out of range.";
        return;
    }

    if (d->currVal == value)
        return;

    d->currVal = value;

    update();
}

int SpinnerProgress::value() const {
    return d->currVal;
}

void SpinnerProgress::fadeIn() {
   d->animation->stop();
   disconnect(d->animation, &QPropertyAnimation::finished, nullptr, nullptr);

   d->animation->setStartValue(0.0);
   d->animation->setEndValue(1.0);

   show();   
   
   d->animation->start();
}

void SpinnerProgress::fadeOut() {
   d->animation->stop();
   disconnect(d->animation, &QPropertyAnimation::finished, nullptr, nullptr);

   d->animation->setStartValue(1.0);
   d->animation->setEndValue(0.0);
   connect(d->animation, &QPropertyAnimation::finished, this, &QWidget::hide);

   d->animation->start();
}

void SpinnerProgress::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);

    const int margin = 10;
    const int diameter = qMin(width(), height()) - margin;
    const qreal recX = (width() - diameter) / 2.0;
    const qreal recY = (height() - diameter) / 2.0;
    
    // Rings Pen
    const qreal strokeW = diameter * 0.06;
    QPen pen;
    pen.setColor(QColor("#D2D4D5"));
    pen.setWidthF(strokeW);
    pen.setCapStyle(Qt::SquareCap);

    // Outer Ring 
    const QRectF outerRect(recX, recY, diameter, diameter);

    painter.save();
    pen.setStyle(Qt::DashLine);
    pen.setDashPattern({4, 3});
    painter.setPen(pen);

    painter.translate(outerRect.center());
    painter.rotate(d->outerAngle);
    painter.translate(-outerRect.center());
    painter.drawArc(outerRect, 0, 360 * 16);
    painter.restore(); 

    // Inner Ring
    const qreal spacing = diameter * 0.2;
    const QRectF innerRect(outerRect.adjusted(spacing,spacing, -spacing, -spacing));

    painter.save();
    pen.setColor(QColor("#14BDE5"));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    if (indeterminate()) {
        painter.translate(innerRect.center());
        painter.rotate(d->innerAngle);
        painter.translate(-innerRect.center());
        painter.drawArc(innerRect, 135 * 16, -270 * 16); 
    } else {
        const qreal spanAngle = (360 * 16) * (value() / 100.0);
        painter.drawArc(innerRect, 90 * 16, -spanAngle);

        // Text
        QFont font("Segoe UI");
        font.setPixelSize(diameter * 0.17);
        painter.setFont(font);

        QFontMetrics fm(font);

        const QString percentText = QString::number(value()) + "%";
        const int textX = innerRect.left() + (innerRect.width() - fm.horizontalAdvance(percentText)) / 2;
        const int textY = innerRect.top() + (innerRect.height() - fm.height()) / 2 + fm.ascent();
        painter.drawText(textX, textY, percentText);
    }

    painter.restore(); 

}
