#include "datamanager.h"
#include "opencv2/opencv.hpp"
#include "x2.pb.h"

const std::vector<std::pair<int, int>> skeleton_pair_ = {\
  {16, 14}, {14, 12}, {17, 15}, {15, 13}, {12, 13}, {6, 12}, {7, 13}, \
  {6, 7}, {6, 8}, {7, 9}, {8, 10}, {9, 11}, {2, 3}, {1, 2}, {1, 3}, \
  {2, 4}, {3, 5}, {4, 6}, {5, 7}};

DataManager::DataManager()
{
    thread_pool_.reset(new hobot::CThreadPool());
    thread_pool_->CreatThread(1); // 创建线程，如果机器核数较少，可适当降低
    data_receiver_.reset(new X2DataReceiver());
    data_receiver_->Init("10.31.41.96:5560");
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
    int errorcode = 0;
    auto result = data_receiver_->RecvData(errorcode);
    if(result.size() <= 0 && errorcode < 0) {
        std::cout << "no msg recv; error code is " << errorcode << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        thread_pool_->PostTask(std::bind(&DataManager::OnX2Data, this));
        return;
    }
    if(result.size() < 2) {
        std::cout << "error: result.size() ==  " << result.size() << std::endl;
        thread_pool_->PostTask(std::bind(&DataManager::OnX2Data, this));
        return;
    }

    // test ////////////////////////////////
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
        std::string type = "jpg";
        if(type == "jpg")
        {
            //img_mat = cv::imread(path);
        }
        if(type == "yuv")
        {}

        std::cout << count << " time:" << end - begin <<std::endl;
    // test end //////////////////////////
    // 解析数据
    x2::SmartFrameMessage smart;
    smart.ParseFromArray(result[0]->data(),result[0]->size());

    cv::Mat img_mat;
    img_mat.create(540 * 3 / 2, 960, CV_8UC1);
    memcpy(img_mat.data, result[1]->data(), 960*540*3/2);
    //cv::cvtColor(img_mat, img_mat, CV_YUV2RGB_NV12);
    cv::cvtColor(img_mat, img_mat, CV_YUV2BGR_NV12);
    std::string w_path = std::to_string(count);
    w_path+=".jpg";
    cv::imwrite(w_path, img_mat);

    // test
    for(int i = 0; i < smart.targets__size(); i++) {
        auto target = smart.targets_(i);
        for(int box_i = 0; box_i < target.boxes__size(); box_i++) {
            auto box = target.boxes_(box_i);
            cv::Point top_left(box.top_left_().x_()/2, box.top_left_().y_()/2);
            cv::Point bottom_right(box.bottom_right_().x_()/2, box.bottom_right_().y_()/2);
            if(box.type_().compare("face") == 0) {
                cv::rectangle(img_mat, top_left, bottom_right, cv::Scalar(255, 0, 0), 1, 8);
            }
            if(box.type_().compare("head") == 0) {
                cv::rectangle(img_mat, top_left, bottom_right, cv::Scalar(0, 255, 0), 1, 8);
            }
            if(box.type_().compare("body") == 0) {
                cv::rectangle(img_mat, top_left, bottom_right, cv::Scalar(0, 0, 255), 1, 8);
            }
        }
        float skeleton_thres_ = 0.2;
        for(int kps_i = 0; kps_i < target.points__size(); kps_i++) {
            auto kps = target.points_(kps_i);
            for(int kps_j = 0; kps_j < kps.points__size(); kps_j++) {
                if(kps.points_(kps_j).score_() > skeleton_thres_) {
                    cv::Point point(kps.points_(kps_j).x_()/2, kps.points_(kps_j).y_()/2);
                    cv::circle(img_mat, point, 2, cv::Scalar(255, 255, 255), -1);
                }
            }
            for (auto &line : skeleton_pair_) {
                  int start = line.first - 1;
                  int end = line.second - 1;
                  const auto score_start = kps.points_(start).score_();
                  const auto score_end = kps.points_(end).score_();
                  if (score_start > skeleton_thres_ && score_end > skeleton_thres_) {
                      const auto start_x = (int) kps.points_(start).x_()/2;
                      const auto start_y = (int) kps.points_(start).y_()/2;
                      const auto end_x = (int) kps.points_(end).x_()/2;
                      const auto end_y = (int) kps.points_(end).y_()/2;
                      cv::line(img_mat, cv::Point(start_x, start_y), cv::Point(end_x, end_y), cv::Scalar(0, 165, 255), 1);
                  }
            }
        }
    }

    // test end

    auto image = QImage((const uchar*)(img_mat.data), img_mat.cols, img_mat.rows, img_mat.cols * img_mat.channels(), QImage::Format_RGB888);
    QPixmap* pix = new QPixmap();
    pix->convertFromImage(image);

    {
        std::lock_guard<std::mutex> lck(mutex_pixmap_);
        pixmap_.reset(pix);
    }
    thread_pool_->PostTask(std::bind(&DataManager::OnX2Data, this));
}
