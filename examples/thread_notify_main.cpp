#include <chrono>
#include <iostream>
#include <thread>
#include "q/thread_exit_notifier.hpp"

void coutThreadExit(std::thread::id id) {
   std::cout << "Thread " << id << " has exited." << std::endl;
}

void threadRun() {
   thread_local ThreadExitNotifier notifier(coutThreadExit);
   for (int i = 0; i < 5; ++i) {
      std::cout << "Thread " << std::this_thread::get_id() << " is running iteration " << i << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }
}

int main() {
   {
      std::thread thread1(threadRun);
      std::thread thread2(threadRun);

      thread1.join();
      thread2.join();
   }
   return 0;
}