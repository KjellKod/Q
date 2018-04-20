/** ==========================================================================
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* ==========================================================================
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
*
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================
*
* Example of a normal std::queue protected by a mutex for operations,
* making it safe for thread communication, using std::mutex from C++0x with
* the help from the std::thread library from JustSoftwareSolutions
* ref: http://www.stdthread.co.uk/doc/headers/mutex.html
*
* This exampel  was totally inspired by Anthony Williams lock-based data structures in
* Ref: "C++ Concurrency In Action" http://www.manning.com/williams */

#pragma once

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

/** Multiple producer, multiple consumer (mpmc) thread safe queue
* protected by mutex. Since 'return by reference' is used this queue won't throw */
namespace mpmc {
   template<typename T>
   class shared_queue {
      std::queue<T> queue_;
      mutable std::mutex m_;
      std::condition_variable data_cond_;
      shared_queue& operator=(const shared_queue&) = delete;
      shared_queue(const shared_queue& other) = delete;

    public:
      shared_queue() {}

      void push(T item) {
         {
            std::lock_guard<std::mutex> lock(m_);
            queue_.push(std::move(item));
         }
         data_cond_.notify_one();
      }

      /// \return immediately, with true if successful retrieval
      bool try_and_pop(T& popped_item) {
         std::lock_guard<std::mutex> lock(m_);
         if (queue_.empty()) {
            return false;
         }
         popped_item = std::move(queue_.front());
         queue_.pop();
         return true;
      }

      /// Try to retrieve, if no items, wait till an item is available and try again
      void wait_and_pop(T& popped_item) {
         std::unique_lock<std::mutex> lock(m_);
         while (queue_.empty()) {
            data_cond_.wait(lock);
            //  This 'while' loop is equal to
            //  data_cond_.wait(lock, [](bool result){return !queue_.empty();});
         }
         popped_item = std::move(queue_.front());
         queue_.pop();
      }

      bool empty() const {
         std::lock_guard<std::mutex> lock(m_);
         return queue_.empty();
      }

      unsigned size() const {
         std::lock_guard<std::mutex> lock(m_);
         return queue_.size();
      }

      unsigned capacity() const {
         std::lock_guard<std::mutex> lock(m_);
         return queue_.capacity();
      }

      unsigned capacity_free() const {
         std::lock_guard<std::mutex> lock(m_);
         return queue_.size() - queue_.capacity();
      }

   };
} // namespace
