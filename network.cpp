#include <network.h>
#include <assert.h>
#include <thread>

X2DataReceiver::X2DataReceiver() {
  context_ = zmq_ctx_new();
  socket_ = nullptr;
  playing_frame_cnt_ = 0;
  reconnect_ = true;
}

X2DataReceiver::~X2DataReceiver() {
  Fini();
  zmq_ctx_destroy(context_);
  context_ = nullptr;
}

void X2DataReceiver::Init(const std::string &endpoint) {
  if (endpoint_ != endpoint) {
    if (endpoint.substr(0, 3) != "tcp") {
      endpoint_ = "tcp://";
      endpoint_ += endpoint;
    } else {
      endpoint_ = endpoint;
    }
    /*int pos = endpoint_.find_last_of(':');
    if (pos == 3) {
      endpoint_ += ":5560";
    }*/
  }

  std::cout << endpoint_ << std::endl;
  
  // Socket to talk to server
  socket_ = zmq_socket(context_, ZMQ_SUB);//

  int recvhwm = 5;
  zmq_setsockopt(socket_, ZMQ_RCVHWM, &recvhwm, sizeof(int));
  //zmq_setsockopt(socket_, ZMQ_RCVTIMEO, &recv_to_, sizeof(int));

  int rc = zmq_connect(socket_, endpoint_.c_str());
  std::cout << "zmq_connect rc:" << rc << std::endl;
  zmq_setsockopt(socket_, ZMQ_SUBSCRIBE, "", 0);
  //assert(rc == 0);

  zmq_msg_init(&recv_msg_);

  recv_faile_count_ = 0;
}

std::vector<std::shared_ptr<std::vector<uint8_t>>> X2DataReceiver::RecvData(int& errorcode) {
  errorcode = 0;
  std::vector<std::shared_ptr<std::vector<uint8_t>>> result;
  int recv_time_out = 5000; // (ms)

  if (recv_faile_count_ > 10000 / recv_time_out) {
    std::cout << "ReConnect()" << std::endl;
    ReConnect();
  }

  zmq_setsockopt(socket_, ZMQ_RCVTIMEO, &recv_time_out, sizeof(int));

  int more = 1;
  size_t more_size = sizeof(more);

  std::vector<zmq_msg_t> vec_zmq_msg;
  do {
    zmq_msg_t msg;
    int rc = zmq_msg_init(&msg);
    assert(rc == 0);

    rc = zmq_msg_recv(&msg, socket_, 0);
    if (rc == -1) {
      recv_faile_count_++;
      reconnect_ = true;
      errorcode = -1;
      return result;
    }

    std::cout << "zmq_msg_recv rc == " << rc << std::endl;

    recv_faile_count_ = 0;

    vec_zmq_msg.push_back(msg);
    std::cout << "vec_zmq_msg.push_back(msg);:" << zmq_msg_size(&msg) << std::endl;

    zmq_getsockopt(socket_, ZMQ_RCVMORE, &more, &more_size);
  } while (more);

  for (size_t i = 0; i < vec_zmq_msg.size(); i++) {
    auto sp_data = std::make_shared <std::vector<uint8_t>>(zmq_msg_size(&vec_zmq_msg[i]));

    memcpy(sp_data->data(),zmq_msg_data(&vec_zmq_msg[i]), zmq_msg_size(&vec_zmq_msg[i]));
    result.push_back(sp_data);

    zmq_msg_close(&vec_zmq_msg[i]);
  }

  if (reconnect_) {
    reconnect_ = false;
  } else {
    playing_frame_cnt_++;
  }
  if(result.size() <= 0 ) {
      errorcode = -2;
  }

  return result;
}

void X2DataReceiver::ReConnect() {
  Fini();
  Init(endpoint_.c_str());
}

void X2DataReceiver::Fini() {
  zmq_msg_close(&recv_msg_);
  zmq_close(socket_);
  socket_ = nullptr;
}


