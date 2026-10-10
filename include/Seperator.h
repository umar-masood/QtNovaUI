#pragma once

#include <QWidget>

struct SeperatorPrivate;

class Seperator : public QWidget {
    Q_OBJECT
    
    public:
        explicit Seperator(QWidget *parent = nullptr);

        void setOrientation(Qt::Orientation orientation);
        Qt::Orientation orientation() const;

        void setDarkMode(bool dark);
        bool darkMode() const;

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        std::unique_ptr<SeperatorPrivate> d = nullptr;
};

// Not for public
struct SeperatorPrivate {
    friend class Seperator;

    private:
        Qt::Orientation orientation = Qt::Vertical;
        bool darkMode = false;
};