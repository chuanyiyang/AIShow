//
// Created by chuanyi.yang@hobot.cc on 06/15/2018.
// Copyright (c) 2018 horizon robotics. All rights reserved.
//
#include "threadpool.h"
namespace hobot {
CThreadPool::CThreadPool() {
  stop_ = false;
}

CThreadPool::~CThreadPool() {
  stop_ = true;
  m_varCondition.notify_all();
  std::lock_guard<std::mutex> lck(m_mutThread);
  for (int i = 0; i < m_nMaxThreads; ++i) {
    m_vecThreads[i]->join();
  }
}

void hobot::CThreadPool::CreatThread(int threadCount) {
  std::lock_guard<std::mutex> lck(m_mutThread);
  m_nMaxThreads = threadCount;
  m_nNumRunningThreads = 0;
  m_vecThreads.reserve(m_nMaxThreads);
  for (int i = 0; i < m_nMaxThreads; ++i) {
    auto thread = std::shared_ptr<std::thread>(new std::thread(
        std::bind(&CThreadPool::exec_loop, this)));
    m_vecThreads.push_back(thread);
  }
  //  wait all threads to start , enter main loop
  while (m_nNumRunningThreads < (int) m_vecThreads.size()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void CThreadPool::exec_loop() {
  ++m_nNumRunningThreads;
  std::shared_ptr<Task> tsk;
  do {
    {
      std::unique_lock<std::mutex> lck(m_mutTaskQuene);
      if (m_setTaskQuenes.size() <= 0) {
        m_varCondition.wait(lck);
      }

      if (m_setTaskQuenes.size() <= 0) {
        continue;
      }
      tsk = m_setTaskQuenes.front();
      m_setTaskQuenes.pop_front();
    }
    //  Exec one task, wake other threads.
    tsk->func();
    m_varCondition.notify_one();
  } while (!stop_);
}

void CThreadPool::PostTask(const TaskFunction &fun) {
  {
    std::lock_guard<std::mutex> lck(m_mutTaskQuene);
    auto task = std::shared_ptr<Task>(new Task(fun));
    m_setTaskQuenes.push_back(task);
  }
  m_varCondition.notify_one();  // wake worker thread(s)
}

void CThreadPool::ClearTask() {
  std::lock_guard<std::mutex> lck(m_mutTaskQuene);
  m_setTaskQuenes.clear();
}

}  // namespace hobot
