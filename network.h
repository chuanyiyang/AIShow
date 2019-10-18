#ifndef UTILS_NETWORK_H_
#define UTILS_NETWORK_H_

#include <string>

#include <zmq.h>
#include <opencv2/opencv.hpp>

class X2DataReceiver {
public:
  X2DataReceiver();
  ~X2DataReceiver();

  void Init(const std::string &endpoint);

  void Fini();

  /**
  * FPGA  retuan cv::Mat
  * SOC   return jpeg
  **/
  std::vector<std::shared_ptr<std::vector<uint8_t>>> RecvData(int& errorcode);
private:
  void ReConnect();

  void *  context_ = nullptr;      // ZMQ Context
  void *  socket_ = nullptr;    // ZMQ Socket encapsulation
  std::string endpoint_ = "";    // ZMQ TCP IP
  zmq_msg_t recv_msg_;
  int recv_faile_count_ = 0;
  int64_t playing_frame_cnt_ = 0;
  bool reconnect_ = false;
};


#endif  // UTILS_NETWORK_H_
