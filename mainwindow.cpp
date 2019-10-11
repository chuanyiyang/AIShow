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
    /*static int count = 0;
    static time_t begin = time(nullptr);
    time_t end = time(nullptr);
    count++;
    std::cout << count << " time:" << end - begin <<std::endl;
    QImage image;
    if(count % 5 == 0)
        image.load("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000139.jpg");
    if(count % 5 == 1)
        image.load("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000285.jpg");
    if(count % 5 == 2)
        image.load("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000632.jpg");
    if(count % 5 == 3)
        image.load("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000724.jpg");
    if(count % 5 == 4)
        image.load("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000785.jpg");*/
    auto pixmap = data_manager_->GetShowImg();
    if(pixmap)
        ui->label->setPixmap(*pixmap.get());
}

