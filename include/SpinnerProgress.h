#pragma once

#include <QWidget>

class QTimer;
class QPropertyAnimation;
class SmoothOpacity;
struct SpinnerProgressPrivate;

class SpinnerProgress : public QWidget {
    Q_OBJECT

    public:
        explicit SpinnerProgress(QWidget *parent = nullptr);

        void start();
        bool started() const;

        void stop();
    
        // void setDarkMode(bool dark); // We may use in future
        // bool darkMode() const;

        void setIndeterminate(bool indeterminate);
        bool indeterminate() const;

        void setRange(int min, int max);

        void setValue(int value);
        int value() const;

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        std::unique_ptr<SpinnerProgressPrivate> d = nullptr;

        void fadeIn();
        void fadeOut();
};

struct SpinnerProgressPrivate {
    friend class SpinnerProgress;
    private:
        bool darkMode = false, indeterminate = false, started = false;
        int outerAngle = 0, innerAngle = 0, min = 0, max = 0, currVal = 0;

        QTimer *timer = nullptr;        
        QPropertyAnimation *animation = nullptr;
        SmoothOpacity *opacity = nullptr;
};