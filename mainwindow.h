#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datamanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnUpdateTimer();
private:
    void StartTimer();
private:
    Ui::MainWindow *ui = nullptr;
    QTimer *timer_ = nullptr;
    std::shared_ptr<DataManager> data_manager_;
};
#endif // MAINWINDOW_H
