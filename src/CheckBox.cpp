#include "../include/CheckBox.h"

#include <QMouseEvent>
#include <QPainter>
#include <QEnterEvent>
#include <QEvent>
#include <QPropertyAnimation>

CheckBox::CheckBox(QWidget *parent) : QWidget(parent), d(std::make_unique<CheckBoxPrivate>()) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(d->checkBoxW , d->checkBoxH); // Default size is 22x22 if no text is set

    // Scale Anim
    d->checkMarkOffsetAnim = new QPropertyAnimation(this, "checkMarkOffset", this);
    d->checkMarkOffsetAnim->setDuration(1000);
    d->checkMarkOffsetAnim->setEasingCurve(QEasingCurve::InOutQuad);
}

void CheckBox::setChecked(bool checked) {
    if (d->checked == checked)
        return;

    d->checked = checked;

    if (d->checked) {
        // Check Mark Movement
        d->checkMarkOffsetAnim->stop();
        d->checkMarkOffsetAnim->setStartValue(0.0);
        d->checkMarkOffsetAnim->setEndValue(1.0);
        d->checkMarkOffsetAnim->start();
    } else {
        d->checkMarkOffsetAnim->stop();
        d->checkMarkOffset = 0.0;
    }

    update();
}

bool CheckBox::checked() const {
    return d->checked;
}

void CheckBox::setDarkMode(bool dark) {
    if (d->darkMode == dark)
        return;

    d->darkMode = dark;

    update();
}

bool CheckBox::darkMode() const {
    return d->darkMode;
}

void CheckBox::setText(const QString &text) {
    if (d->text == text)
        return;

    d->text = text;
    d->fm = QFontMetrics(font());

    d->textW = d->fm.horizontalAdvance(d->text);
    d->textH = d->fm.height();

    const int totalW = d->checkBoxW + d->spacing + d->textW;
    const int totalH = std::max(d->textH, d->checkBoxH);
    setFixedSize(totalW, totalH);

    update();
}

QString CheckBox::text() const {
    return d->text;
}

bool CheckBox::hovered() const {
    return d->hovered;
}

void CheckBox::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // CheckBox
    const int checkBoxRectX = 0;
    const int checkBoxRectY = (height() - d->checkBoxH) / 2;
    QRect checkBoxRect = QRect(checkBoxRectX, checkBoxRectY, d->checkBoxW, d->checkBoxH).adjusted(1, 1, -1, -1);

    // Border
    QPen pen;
    pen.setWidthF(checked() ? 1.0 : 0.5);
    pen.setColor(checked() ? QColor("#0191DF") : (darkMode() ? QColor("#4D4D4D") : QColor("#CCCCCC")));
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    // Background
    QColor brushColor;
    if (checked()) 
       brushColor = hovered() ? QColor("#1BB3E6") : QColor("#0191DF");
    else if (hovered()) 
       brushColor = darkMode() ? QColor("#323232") : QColor("#F0F0F0");
    else 
       brushColor = darkMode() ? QColor("#2D2D2D") : QColor("#FBFBFB");
    
    painter.setBrush(brushColor);
    painter.drawRoundedRect(checkBoxRect, 6, 6);

    // Drawing Check Mark
    QRect checkMarkRect(checkBoxRect.adjusted(6, 6, -6, -6));
    QPointF p1(checkMarkRect.left(), checkMarkRect.top() + 5.0);
    QPointF p2(checkMarkRect.left() + 2.5, checkMarkRect.bottom());
    QPointF p3(checkMarkRect.right() + 1.0, checkMarkRect.top()); // +1.0 for ui tweak of check mark
    
    painter.setPen(QPen(Qt::white, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    // First Stroke
    qreal firstOffset = (d->checkMarkOffset / 0.4);
    if (firstOffset > 1.0)
        firstOffset = 1.0;

    QPointF currP = p1 + (p2 - p1) * firstOffset; 
    painter.drawLine(p1, currP);        

    // Second Stroke
    if (d->checkMarkOffset > 0.4) {
        qreal secondOffset = (d->checkMarkOffset - 0.4) / 0.6;
        QPointF currP = p2 + (p3 - p2) * secondOffset;
        painter.drawLine(p2, currP);
    }

    // Text
    if (!text().isEmpty()) {
        painter.setPen(darkMode() ? Qt::white : Qt::black);
        const int textRectX = checkBoxRect.right() + d->spacing;
        const int textRecY = checkBoxRect.top() + (checkBoxRect.height() - d->textH) / 2 + d->fm.ascent();
        painter.drawText(textRectX, textRecY, text());
    }
}

bool CheckBox::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);
            if (mEvent->button() == Qt::LeftButton) {
                setChecked(!checked());
                emit checked(d->checked);
            }
        }
        break;

        case QEvent::Enter:
            d->hovered = true;
            update();
        break;

        case QEvent::Leave:
            d->hovered = false;
            update();
        break;
    }

    return QWidget::event(event);
}

void CheckBox::setCheckMarkOffset(qreal offset) {
    if (d->checkMarkOffset == offset)
        return;

    d->checkMarkOffset = offset;
    
    update();
}

qreal CheckBox::checkMarkOffset() const {
    return d->checkMarkOffset;
}
