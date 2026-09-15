#include "../include/ScrollBar.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

ScrollBar::ScrollBar(QWidget *parent) : QScrollBar(parent), 
                                        d(std::make_unique<ScrollBarPrivate>()) {
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
        case QEvent::Enter:
            d->hovered = true;
            update();
        break;

        case QEvent::Leave:
            d->hovered = false;
            update();
        break;

        case QEvent::MouseButtonPress:
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) 
                d->pressed = true;
            update();
        break;

        case QEvent::MouseButtonRelease:
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) 
                d->pressed = false;
            update();
        break;
    };

    return QScrollBar::event(event);
}

void ScrollBar::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.setRenderHints(QPainter::Antialiasing);


}