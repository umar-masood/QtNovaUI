#include "../include/Switch.h"

#include <QMouseEvent>
#include <QPainter>
#include <QEnterEvent>
#include <QEvent>
#include <QPropertyAnimation>

Switch::Switch(QWidget *parent) : QWidget(parent), d(std::make_unique<SwitchPrivate>()) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(d->width, d->height); // Changing size can cause ui misalignment

    // Scale Anim
    d->knobScaleAnim = new QPropertyAnimation(this, "knobScale", this);
    d->knobScaleAnim->setDuration(300);
    d->knobScaleAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Offset Anim
    d->knobOffsetAnim = new QPropertyAnimation(this, "knobOffset", this);
    d->knobOffsetAnim->setDuration(300);
    d->knobOffsetAnim->setEasingCurve(QEasingCurve::InOutCubic);

    // Shrink/Stretch Anim
    d->knobWidthAnim = new QPropertyAnimation(this, "knobWidth", this);
    d->knobWidthAnim->setDuration(600);
    d->knobWidthAnim->setEasingCurve(QEasingCurve::OutQuad);
}

void Switch::setChecked(bool checked) {
    if (d->checked == checked)
        return;

    d->checked = checked;

    if (d->checked) {
        // Knob Movement (Left -> Right)
        d->knobOffsetAnim->stop();
        d->knobOffsetAnim->setStartValue(0.0);
        d->knobOffsetAnim->setEndValue(1.0);
        d->knobOffsetAnim->start();
    } else {
        // Knob Movement (Right -> Left)
        d->knobOffsetAnim->stop();
        d->knobOffsetAnim->setStartValue(1.0);
        d->knobOffsetAnim->setEndValue(0.0);
        d->knobOffsetAnim->start();
    }

    // Knob Shrink/Expand
    d->knobWidthAnim->stop();
    d->knobWidthAnim->setKeyValueAt(0.0, 14);
    d->knobWidthAnim->setKeyValueAt(0.2, 20);
    d->knobWidthAnim->setKeyValueAt(0.4, 22);
    d->knobWidthAnim->setKeyValueAt(0.6, 26);
    d->knobWidthAnim->setKeyValueAt(0.8, 22);
    d->knobWidthAnim->setKeyValueAt(1.0, 14);
    d->knobWidthAnim->start();

    emit toggled(d->checked);
    update();
}

bool Switch::checked() const {
    return d->checked;
}

void Switch::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool Switch::darkMode() const {
    return d->darkMode;
}

bool Switch::hovered() const {
    return d->hovered;
}

void Switch::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rec = QRect(rect().x(), rect().y(), d->width, d->height).adjusted(1, 1, -1, -1);

    // Background
    QColor brushColor;
    if (checked()) 
        brushColor = hovered() ? QColor("#1BB3E6") : QColor("#109AC7");
    else if (hovered()) 
        brushColor = darkMode() ? QColor("#2D2D2D") : QColor("#E4E4E4");
    else
        brushColor = darkMode() ? QColor("#202020") : QColor("#ECECEC");
    
    painter.setBrush(brushColor);

    // Border
    QColor borderPen = darkMode() ? QColor("#9F9F9F") : QColor("#8A8A8A");
    painter.setPen(checked() ? QColor("#109AC7") : borderPen);

    painter.drawRoundedRect(rec, rec.height() / 2, rec.height() / 2);

    // Knob
    QColor knobBrush;
    if (checked())
        knobBrush = darkMode() ? QColor("#FFFFFF") : QColor("#000000");
    else
        knobBrush = darkMode() ? QColor("#9F9F9F") : QColor("#8A8A8A");
 
    painter.setPen(Qt::NoPen);
    painter.setBrush(knobBrush);

    const int spacing = 4;
    const int knobRectW = d->knobWidth;
    const int knobRectH = 14;
    const int knobRange = width() - (2 * spacing) - knobRectW;
    const int knobRectX = spacing + d->knobOffset * knobRange;
    const int knobRectY = rec.top() + (rec.height() - knobRectH) / 2;

    QRect knobRect(knobRectX, knobRectY, knobRectW, knobRectH);

    // Scaling Knob
    painter.translate(knobRect.center());
    painter.scale(d->knobScale, d->knobScale);
    painter.translate(-knobRect.center());
    painter.drawEllipse(knobRect);
}

void Switch::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) 
        setChecked(!checked());
}

void Switch::enterEvent(QEnterEvent *event) {
    d->hovered = true;

    // Knob Scale In Anim
    d->knobScaleAnim->stop();
    d->knobScaleAnim->setStartValue(1.0);
    d->knobScaleAnim->setEndValue(1.15);
    d->knobScaleAnim->start();

    update();
}

void Switch::leaveEvent(QEvent *event) {
    d->hovered = false;

    // Knob Scale Out Anim
    d->knobScaleAnim->stop();
    d->knobScaleAnim->setStartValue(1.15);
    d->knobScaleAnim->setEndValue(1.0);
    d->knobScaleAnim->start();

    update();
}

void Switch::setKnobWidth(int width) {
    if (d->knobWidth == width)
        return;

    d->knobWidth = width;
    
    update();
}

int Switch::knobWidth() const {
    return d->knobWidth;
}

void Switch::setKnobScale(qreal scale) {
    if (d->knobScale == scale)
        return;

    d->knobScale = scale;
    
    update();
}

qreal Switch::knobScale() const {
    return d->knobScale;
}

void Switch::setKnobOffset(qreal offset) {
    if (d->knobOffset == offset)
        return;

    d->knobOffset = offset;
    
    update();
}

qreal Switch::knobOffset() const {
    return d->knobOffset;
}
