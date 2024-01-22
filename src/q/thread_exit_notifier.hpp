#pragma once

#include <functional>
#include <thread>

/** 
 * Simple RAII to trigger a notification when the thread exits
 * This is used here to be able to do registring and cleanup of queues that are no longer active with a thread
*/
class ThreadExitNotifier {
  public:
   std::thread::id id_;
   std::function<void(std::thread::id)> notifyMe_;

   ThreadExitNotifier(std::function<void(std::thread::id)> notifyme) :
       id_(std::this_thread::get_id()),
       notifyMe_(notifyme) {
   }

   ~ThreadExitNotifier() {
      notifyMe_(id_);
   }
};
