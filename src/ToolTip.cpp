#include "../include/ToolTip.h"

#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QApplication>
#include <QScreen>

ToolTip::ToolTip(const QString &text, QWidget *target, QWidget *parent) 
                : QWidget(parent), d(std::make_unique<ToolTipPrivate>()) {
    
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint |Qt::WindowDoesNotAcceptFocus);

    installEventFilter(this);
    hide();

    setText(text);
    setTargetWidget(target);

    // Show/Hide Timer
    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    d->timer->setInterval(2000);
    connect(d->timer, &QTimer::timeout, this, &ToolTip::onTimeout);
    
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
    setGraphicsEffect(opacityEffect);

    // Fade In Animation
    d->fadeInAnim = new QPropertyAnimation(opacityEffect, "opacity");
    d->fadeInAnim->setEasingCurve(QEasingCurve::InOutQuad);
    d->fadeInAnim->setDuration(300);

    // Fade Out Animation
    d->fadeOutAnim = new QPropertyAnimation(opacityEffect, "opacity");
    d->fadeOutAnim->setEasingCurve(QEasingCurve::InOutQuad);
    d->fadeOutAnim->setDuration(300);
    connect(d->fadeOutAnim, &QPropertyAnimation::finished, this, &QWidget::hide);
}

void ToolTip::setText(const QString &text) {
    if (d->text == text)
        return;

    d->text = text;

    update();
}

QString ToolTip::text() const {
    return d->text;
}

void ToolTip::setTargetWidget(QWidget *target) {
    if (!target || d->targetWidget == target)
        return;

    if (d->targetWidget)
        d->targetWidget->removeEventFilter(this);

    d->targetWidget = target;
    d->targetWidget->installEventFilter(this);
}

QWidget* ToolTip::targetWidget() const {
    return d->targetWidget;
}

void ToolTip::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool ToolTip::darkMode() const {
    return d->darkMode;
}

QSize ToolTip::sizeHint() const {
    QFontMetrics fm(font());
    int textW = fm.horizontalAdvance(text());
    int textH = fm.height();

    const int padding = 8;
    QSize rectSize = fm.boundingRect(0, 0, 400, 0, Qt::TextWordWrap, text()).size();

    return QSize(2 * padding + rectSize.width(), 2 * padding + rectSize.height());
} 

void ToolTip::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    
    QRect rec(rect().adjusted(1, 1, -1, -1));

    // Background
    painter.setPen(QPen(QColor("#AFAFAF"), 0.6));
    painter.setBrush(darkMode() ? QColor("#242424") : QColor("#FFFFFF"));
    painter.drawRoundedRect(rec, 6, 6);

    // Text
    painter.setPen(darkMode() ? QColor("#FFFFFF") : QColor("#000000"));

    const int padding = 8;
    QRect textRect(padding, 0, width() - 2 * padding, height());
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, text());
}

void ToolTip::onTimeout() {
    if (targetWidget() && targetWidget()->underMouse()) {
        position();
        fadeIn();
    }
}

void ToolTip::position() {
    if (!targetWidget())
        return;

    const QPoint targetWidgetGlobalPos = targetWidget()->mapToGlobal(QPoint(0,0));
    const QRect targetWidgetRect(targetWidgetGlobalPos, targetWidget()->size());

    QScreen *screen = QApplication::screenAt(targetWidgetGlobalPos);
    if (!screen)
        screen = QApplication::primaryScreen();
        
    QRect screenRect = screen->availableGeometry();

    const QPoint abovePos(targetWidgetRect.left() + (targetWidgetRect.width() - sizeHint().width()) / 2, 
                          targetWidgetRect.top() - sizeHint().height() - 3);

    const QPoint belowPos(targetWidgetRect.left() + (targetWidgetRect.width() - sizeHint().width()) / 2,
                          targetWidgetRect.bottom() + 3);

    const QPoint leftPos(targetWidgetRect.left() - sizeHint().width() - 3,
                         targetWidgetRect.top() + (targetWidgetRect.height() - sizeHint().height()) / 2);

    const QPoint rightPos(targetWidgetRect.right() + 3,
                         targetWidgetRect.top() + (targetWidgetRect.height() - sizeHint().height()) / 2);
    
    QPoint currPos;
    if (screenRect.contains(QRect(abovePos, size())))
        currPos = abovePos;
    else if (screenRect.contains(QRect(belowPos, size())))
        currPos = belowPos;
    else if (screenRect.contains(QRect(leftPos, size())))
        currPos = leftPos;
    else if (screenRect.contains(QRect(rightPos, size())))
        currPos = rightPos;
    else
        currPos = belowPos;

    const int x = std::clamp(currPos.x(), screenRect.left(), screenRect.right() - sizeHint().width());
    const int y = std::clamp(currPos.y(), screenRect.top(), screenRect.bottom() - sizeHint().height());

    move(x, y);
}

void ToolTip::fadeIn() {
    d->fadeInAnim->stop();

    d->fadeInAnim->setStartValue(0.0);
    d->fadeInAnim->setEndValue(1.0);
  
    show();
    d->fadeInAnim->start();
}

void ToolTip::fadeOut() {
    d->fadeOutAnim->stop();

    d->fadeOutAnim->setStartValue(1.0);
    d->fadeOutAnim->setEndValue(0.0);
    
    d->fadeOutAnim->start();
}

bool ToolTip::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ApplicationStateChange) {
        d->timer->stop();
        fadeOut();
    }

    if (obj == targetWidget()) {
        switch (event->type()) {
            case QEvent::Enter:
                d->timer->start();
            break;

            case QEvent::Leave:	
                d->timer->stop();
                QTimer::singleShot(150, this, [this](){
                    if (!targetWidget() || (!targetWidget()->underMouse() && !underMouse()))
                        fadeOut();
                });
            break;

            case QEvent::Hide:
            case QEvent::Destroy:
                d->timer->stop();
                fadeOut();
            break;
        }
    }

    if (obj == this && event->type() == QEvent::Leave) {
        QTimer::singleShot(150, this, [this]() {
            if (!targetWidget() || (!targetWidget()->underMouse() && !underMouse()))
                fadeOut();
        });
    }
  
    return QWidget::eventFilter(obj, event);
}