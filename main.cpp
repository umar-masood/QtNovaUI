#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

#include "./include/SpinnerProgress.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("SpinnerProgress Test");
    window.resize(500, 420);

    auto *mainLayout = new QVBoxLayout(&window);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Spinner
    auto *spinner = new SpinnerProgress;
    spinner->setFixedSize(180, 180);

    mainLayout->addWidget(spinner, 0, Qt::AlignCenter);

    // Value label
    auto *valueLabel = new QLabel("Value: 50%");
    valueLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(valueLabel);

    // Slider
    auto *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);

    mainLayout->addWidget(slider);

    // Buttons layout
    auto *buttonsLayout = new QHBoxLayout;

    auto *startButton = new QPushButton("Start");
    auto *stopButton = new QPushButton("Stop");

    auto *determinateButton =
        new QPushButton("Determinate");

    auto *indeterminateButton =
        new QPushButton("Indeterminate");

    buttonsLayout->addWidget(startButton);
    buttonsLayout->addWidget(stopButton);
    buttonsLayout->addWidget(determinateButton);
    buttonsLayout->addWidget(indeterminateButton);

    mainLayout->addLayout(buttonsLayout);

    // Initial setup
    spinner->setRange(0, 100);
    spinner->setValue(50);
    spinner->setIndeterminate(false);

    // Start
    QObject::connect(
        startButton,
        &QPushButton::clicked,
        spinner,
        &SpinnerProgress::start
    );

    // Stop
    QObject::connect(
        stopButton,
        &QPushButton::clicked,
        spinner,
        &SpinnerProgress::stop
    );

    // Determinate mode
    QObject::connect(
        determinateButton,
        &QPushButton::clicked,
        [spinner, slider]()
        {
            spinner->setIndeterminate(false);
            spinner->setValue(slider->value());
            spinner->start();
        }
    );

    // Indeterminate mode
    QObject::connect(
        indeterminateButton,
        &QPushButton::clicked,
        [spinner]()
        {
            spinner->setIndeterminate(true);
            spinner->start();
        }
    );

    // Slider value
    QObject::connect(
        slider,
        &QSlider::valueChanged,
        [spinner, valueLabel](int value)
        {
            valueLabel->setText(
                QString("Value: %1%").arg(value)
            );

            if (!spinner->indeterminate())
                spinner->setValue(value);
        }
    );

    window.show();

    return app.exec();
}