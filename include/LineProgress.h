#pragma once

#include <QWidget>

class QTimer;
class QPropertyAnimation;
class SmoothOpacity;
class QGraphicsOpacityEffect;
struct LineProgressPrivate;

class LineProgress : public QWidget {
    Q_OBJECT

    public:
        explicit LineProgress(QWidget *parent = nullptr);

        void start();
        bool started() const;

        void stop();
    
        void setDarkMode(bool dark); // We may use in future
        bool darkMode() const;

        void setText(const QString &text);
        QString text() const;

        void setIndeterminate(bool indeterminate);
        bool indeterminate() const;

        void setRange(int min, int max);

        void setValue(int value);
        int value() const;

    protected:
        void paintEvent(QPaintEvent *event) override;
        QSize sizeHint() const;

    private:
        std::unique_ptr<LineProgressPrivate> d = nullptr;

        void fadeIn();
        void fadeOut();
};

struct LineProgressPrivate {
    friend class LineProgress;
    private:
        bool darkMode = false, indeterminate = false, started = false;
        int min = 0, max = 100, currVal = 0;
        double offset = 0.0;

        QString text;

        QTimer *timer = nullptr;        
        QPropertyAnimation *fadeAnimation = nullptr;
        QGraphicsOpacityEffect *opacity = nullptr;
};