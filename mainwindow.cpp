#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    AttitudeIndicator *ai = ui->centralWidget;
    switch (event->key())
    {
    case Qt::Key_Left:
        if (event->modifiers().testFlag(Qt::ShiftModifier))
            ai->setYaw(ai->getYaw() - 1.);
        else
            ai->setRoll(ai->getRoll() - 1.);
        break;
    case Qt::Key_Right:
        if (event->modifiers().testFlag(Qt::ShiftModifier))
            ai->setYaw(ai->getYaw() + 1.);
        else
            ai->setRoll(ai->getRoll() + 1.);
        break;
    case Qt::Key_Down:
        ai->setPitch(ai->getPitch() - 1.);
        break;
    case Qt::Key_Up:
        ai->setPitch(ai->getPitch() + 1.);
        break;
    }
}
