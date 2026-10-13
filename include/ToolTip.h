#pragma once

#include <QWidget>

struct ToolTipPrivate;
class QTimer;
class QPropertyAnimation;

class ToolTip : public QWidget {
    Q_OBJECT
  
    public:
        explicit ToolTip(const QString &text, QWidget *target = nullptr, QWidget *parent = nullptr);
    
        void setText(const QString &text);
        QString text() const;

        void setDarkMode(bool dark);
        bool darkMode() const;

        void setTargetWidget(QWidget *target);
        QWidget* targetWidget() const;

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;
        void paintEvent(QPaintEvent *event) override;
        QSize sizeHint() const;

    private:
        std::unique_ptr<ToolTipPrivate> d = nullptr;

        void fadeIn();
        void fadeOut();

        void onTimeout();
        void position();
};

// Not for public
struct ToolTipPrivate {
    friend class ToolTip;

    private:
        bool darkMode = false;
        QString text;
        
        QWidget *targetWidget = nullptr;
        QTimer *timer = nullptr;

        QPropertyAnimation *fadeInAnim = nullptr;
        QPropertyAnimation *fadeOutAnim = nullptr;
};