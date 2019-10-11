//
// Created by chuanyi.yang@hobot.cc on 06/15/2018.
// Copyright (c) 2018 horizon robotics. All rights reserved.
//
#ifndef SRC_COMMON_THREADPOOL_H_
#define SRC_COMMON_THREADPOOL_H_

#include <set>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>

namespace hobot {
typedef std::function<void()> TaskFunction;
struct Task {
  TaskFunction func;
  explicit Task(const TaskFunction &_task) : func(_task) {
  }
};

class CThreadPool {
 public:
  CThreadPool();
  virtual ~CThreadPool();
  void CreatThread(int threadCount);
  // post an async task
  void PostTask(const TaskFunction &task);
  void ClearTask();
  // void PostPollingTask(const TaskFunction& task);
  void Stop() {} // �ȴ�ʵ��
  void Start() {} // �ȴ�ʵ��

 protected:
  void exec_loop();

 private:
  typedef std::list<std::shared_ptr<Task> > TaskContainer;
  TaskContainer m_setTaskQuenes;
  mutable std::mutex m_mutThread;
  // a mutex for task quene operations only
  mutable std::mutex m_mutTaskQuene;

  std::condition_variable m_varCondition;
  std::atomic<int> m_nNumRunningThreads;
  typedef std::shared_ptr<std::thread> CThreadPtr;

  std::vector<CThreadPtr> m_vecThreads;
  std::atomic<bool> stop_;
  // cound set the value before starting the thread pool only
  int m_nMaxThreads = 0;
};
};  // namespace hobot
#endif  // SRC_COMMON_THREADPOOL_H_
