#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    StartTimer();
    data_manager_.reset(new DataManager());
    data_manager_->Start("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StartTimer()
{
    // 使用timer去定期刷新界面，而非被动等待底层回调，可以使界面的性能更稳定
    timer_ = new QTimer(this);
    timer_->setTimerType(Qt::PreciseTimer);
    connect(timer_, SIGNAL(timeout()), this, SLOT(OnUpdateTimer()));
    timer_->start(50);
}

void MainWindow::OnUpdateTimer() // 界面线程只做绘制
{
    auto pixmap = data_manager_->GetShowImg();
    if(pixmap)
        ui->label->setPixmap(*pixmap.get());
}

