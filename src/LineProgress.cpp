#include "../include/LineProgress.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>

LineProgress::LineProgress(QWidget *parent) : QWidget(parent) , d(std::make_unique<LineProgressPrivate>()) {  
    setAttribute(Qt::WA_TranslucentBackground);
    hide();

    // Opacity
    d->opacity = new QGraphicsOpacityEffect;
    setGraphicsEffect(d->opacity);    

    // Fade Animation
    d->fadeAnimation = new QPropertyAnimation(d->opacity, "opacity");
    d->fadeAnimation->setDuration(300);
    d->fadeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // Timer
    d->timer = new QTimer(this);
    d->timer->setInterval(10);
    connect(d->timer, &QTimer::timeout, this, [this]() {
        d->offset = std::fmod(d->offset + 0.01, 1.0);
        update();
    });
}

void LineProgress::fadeIn() {
   d->fadeAnimation->stop();

   disconnect(d->fadeAnimation, &QPropertyAnimation::finished, nullptr, nullptr);

   d->fadeAnimation->setStartValue(0.0);
   d->fadeAnimation->setEndValue(1.0);

   d->fadeAnimation->start();
   show();
}

void LineProgress::fadeOut() {
   d->fadeAnimation->stop();

   disconnect(d->fadeAnimation, &QPropertyAnimation::finished, nullptr, nullptr);

   d->fadeAnimation->setStartValue(1.0);
   d->fadeAnimation->setEndValue(0.0);
   
   connect(d->fadeAnimation, &QPropertyAnimation::finished, this, &QWidget::hide);

   d->fadeAnimation->start();
}

void LineProgress::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool LineProgress::darkMode() const {
    return d->darkMode;
}

void LineProgress::setText(const QString &text) {
    if (d->text == text)
        return;

    d->text = text;

    update();
}

QString LineProgress::text() const {
    return d->text;
}

void LineProgress::start() {
    if (!d->timer->isActive())
        d->timer->start(); 

    d->started = true;

    fadeIn();
}

bool LineProgress::started() const {
    return d->started;
}

void LineProgress::stop() {
    d->started = false;

    if (d->timer && d->timer->isActive()) 
        d->timer->stop();

    fadeOut();
}

void LineProgress::setIndeterminate(bool indeterminate) {
    if (d->indeterminate == indeterminate)
        return;

    d->indeterminate = indeterminate;

    update();
}

bool LineProgress::indeterminate() const {
    return d->indeterminate;
}

void LineProgress::setRange(int min, int max) {
    if (min < 0 || max > 100 || min >= max) {
        d->min = 0; d->max = 100;
    } else {
        d->min = min; d->max = max;
    }
}

void LineProgress::setValue(int value) {
    if (value < d->min || value > d->max) {
        qWarning() << "Your provided value is out of range.";
        return;
    }

    if (d->currVal == value)
        return;

    d->currVal = qBound(d->min, value, d->max);

    update();
}

int LineProgress::value() const {
    return d->currVal;
}

QSize LineProgress::sizeHint() const {
    return QSize(250, 50);
}

void LineProgress::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    const int margin = 8;

    // Track
    painter.setPen(Qt::NoPen);
    painter.setBrush(darkMode() ? "#383838" : "#F0F0F0");

    const int trackW = width() - 2 * margin;
    const int trackH = 8;
    const int radius = trackH / 2;

    QRect trackRect(margin, 0, trackW, trackH);
    painter.drawRoundedRect(trackRect, radius, radius);

    // Progress Line
    painter.setBrush("#0191DF");

    const int progressLineW = trackW / 3;
    const int progressLineX = margin + int(d->offset * trackW);
    const int progressLineH = trackH;

    if (indeterminate()) {
        if (progressLineX + progressLineW > trackW + margin) {
            int overflow = (progressLineX + progressLineW) - (trackW + margin);
            painter.drawRoundedRect(QRect(progressLineX, 0, progressLineW - overflow, progressLineH), radius, radius);
            painter.drawRoundedRect(QRect(margin, 0, overflow, progressLineH), radius, radius);
        } else 
            painter.drawRoundedRect(QRect(progressLineX, 0, progressLineW, progressLineH), radius, radius);
      
    } else {
        const double progress = double(d->currVal - d->min) / (d->max - d->min);
        const int currLineProgressW = int(progress * trackW);

        QRect progressLineRect(margin,0, currLineProgressW, progressLineH);
        painter.drawRoundedRect(progressLineRect, radius, radius);
    }

    // --------------- Text -------------------
    QFont font;
    font.setPointSize(10);
    font.setFamily("Segoe UI");
    font.setWeight(QFont::Normal);
    painter.setFont(font);
    painter.setPen(darkMode() ? QColor("#F0F0F0") : QColor("#000000"));

    QRect textRect(0, height() - 25, width(), 30);
    painter.drawText(textRect, Qt::AlignHCenter, d->text);
}
