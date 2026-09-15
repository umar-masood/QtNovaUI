#pragma once

#include <QScrollBar>
#include <QWidget>

struct ScrollBarPrivate;

class ScrollBar : public QScrollBar {
    Q_OBJECT

    public:
        explicit ScrollBar(QWidget *parent = nullptr);

        void setDarkMode(bool dark);
        bool darkMode() const;

    protected:
        bool event(QEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

    private:
        std::unique_ptr<ScrollBarPrivate> d = nullptr;
};

// Not for public use
struct ScrollBarPrivate {
    friend class ScrollBar;

    private:
    bool darkMode = false, hovered = false, pressed = false;
};