#include "datamanager.h"
#include "opencv2/opencv.hpp"

DataManager::DataManager()
{
    thread_pool_.reset(new hobot::CThreadPool());
    thread_pool_->CreatThread(1); // 创建线程，如果机器核数较少，可适当降低
}

int DataManager::Start(std::string X2IP)
{
    StartZmqClient();
    if(!thread_pool_)
        return -1000;
    thread_pool_->PostTask(std::bind(&DataManager::OnX2Data, this));
    return 0;
}

void DataManager::StartZmqClient()
{
     // 待补充
}

std::shared_ptr<QPixmap> DataManager::GetShowImg()
{
    std::shared_ptr<QPixmap> result;
    {
        std::lock_guard<std::mutex> lck(mutex_pixmap_);
        result.swap(pixmap_);
    }
    return result;
}

void DataManager::OnX2Data()
{
    // test
        //std::this_thread::sleep_for(std::chrono::milliseconds(30));
        std::string path;
        static int count = 0;
        static time_t begin = time(nullptr);
        time_t end = time(nullptr);
        count ++;
        if(count % 5 == 0)
            path = ("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000139.jpg");
        if(count % 5 == 1)
            path = ("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000285.jpg");
        if(count % 5 == 2)
            path = ("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000632.jpg");
        if(count % 5 == 3)
            path = ("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000724.jpg");
        if(count % 5 == 4)
            path = ("C:\\Users\\chuanyi.yang\\Documents\\MobaXterm\\home\\demo\\test_coco\\000000000785.jpg");
        thread_pool_->PostTask(std::bind(&DataManager::OnX2Data, this));
        std::cout << count << " time:" << end - begin <<std::endl;
    // test end
    // 解析数据
    std::string type = "jpg";
    cv::Mat img_mat;
    cv::Mat test;
    if(type == "jpg")
    {
        img_mat = cv::imread(path);
    }
    if(type == "yuv")
    {

    }
    cv::cvtColor(img_mat, img_mat, CV_BGR2RGB);
    // test
    cv::Point top_left(100, 100);
    cv::Point bottom_right(300, 300);
    cv::rectangle(img_mat, top_left, bottom_right, cv::Scalar(255, 0, 0), 8, 8);
    // test end

    auto image = QImage((const uchar*)(img_mat.data), img_mat.cols, img_mat.rows, img_mat.cols * img_mat.channels(), QImage::Format_RGB888);
    QPixmap* pix = new QPixmap();
    pix->convertFromImage(image);

    {
        std::lock_guard<std::mutex> lck(mutex_pixmap_);
        pixmap_.reset(pix);
    }
}
