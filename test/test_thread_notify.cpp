
/* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* Originally published at: https://github.com/KjellKod/Q
*/
#include <gtest/gtest.h>
#include <chrono>
#include <future>
#include <mutex>
#include <set>
#include <thread>
#include <q/internal/thread_exit_notifier.hpp>

namespace {
   std::set<std::thread::id> ids;

   void threadExitRegister(std::thread::id id) {
      ids.insert(id);
   }

   bool hasId(std::thread::id id) {
      return ids.find(id) != ids.end();
   }
}  // namespace

TEST(ThreadExitNotifier, Initialization_raii_dummytest) {
   std::thread::id id = std::this_thread::get_id();
   {
      ThreadExitNotifier notifier(threadExitRegister);
   }
   EXPECT_TRUE(hasId(id));
}

TEST(ThreadExitNotifier, async_future_thread_exit) {
   auto future = std::async(std::launch::async, [] {
      std::thread::id id = std::this_thread::get_id();
      ThreadExitNotifier notifier(threadExitRegister);
      return id;
   });

   std::thread::id id = future.get();
   EXPECT_TRUE(hasId(id));
}

TEST(ThreadExitNotifier, thread_exit) {
   std::thread::id id;
   std::mutex idMutex;
   std::condition_variable condition_check;
   bool isRegistrered = false;

   std::thread thread1([&id, &idMutex, &condition_check, &isRegistrered] {
      ThreadExitNotifier notifier([&](std::thread::id exitingId) {
         threadExitRegister(exitingId);
         condition_check.notify_one();
      });  // lambda will be called only when the notfier construct goes out of scope
      {
         std::lock_guard<std::mutex> lock(idMutex);
         id = std::this_thread::get_id();
         isRegistrered = true;
      }
   });

   // Wait for the thread ID to be captured
   std::unique_lock<std::mutex> lock(idMutex);
   condition_check.wait(lock, [&isRegistrered] { return isRegistrered; });

   thread1.join();
   EXPECT_TRUE(hasId(id));
}