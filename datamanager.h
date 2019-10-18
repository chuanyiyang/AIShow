#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QWidget>
#include "threadpool.h"
#include "network.h"

class DataManager
{
public:
    DataManager();

    int Start(std::string X2IP);
    std::shared_ptr<QPixmap> GetShowImg();

private:
    void StartZmqClient();
    void OnX2Data();
private:

    std::shared_ptr<hobot::CThreadPool> thread_pool_;

    std::shared_ptr<QPixmap> pixmap_; // 这里仅保存一帧图像，暂不考虑为平滑性做多帧缓存
    mutable std::mutex mutex_pixmap_;
    std::shared_ptr<X2DataReceiver> data_receiver_;
};

#endif // DATAMANAGER_H
