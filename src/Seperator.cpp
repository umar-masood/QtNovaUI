#include "../include/Seperator.h"

#include <QPainter>

Seperator::Seperator(QWidget *parent) : QWidget(parent), 
                                        d(std::make_unique<SeperatorPrivate>()) {
}

void Seperator::setOrientation(Qt::Orientation orientation) {
    if (d->orientation == orientation)
        return;

    d->orientation = orientation;

    update();
}

Qt::Orientation Seperator::orientation() const {
    return d->orientation;
}

void Seperator::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool Seperator::darkMode() const {
    return d->darkMode;
}

void Seperator::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(darkMode() ? QColor(255, 255, 255, 30) : QColor(0, 0, 0, 30), 1));

    if (orientation() == Qt::Horizontal) 
        painter.drawLine(0, height() / 2, width(), height() / 2);
    else if (orientation() == Qt::Vertical) 
        painter.drawLine(width() / 2, 0, width() / 2, height());
}