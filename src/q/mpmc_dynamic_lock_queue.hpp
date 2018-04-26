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
#include <chrono>


/** Multiple producer, multiple consumer (mpmc) thread safe queue
* protected by mutex. Since 'return by reference' is used this queue won't throw */
namespace mpmc {
   template<typename T>
   class dynamic_lock_queue {
      static const int kUnlimited = -1;
      static const int kSmallDefault = 100;
      const int kMaxSize;
      std::queue<T> queue_;
      mutable std::mutex m_;
      std::condition_variable data_cond_;
   
      dynamic_lock_queue& operator=(const dynamic_lock_queue&) = delete;
      dynamic_lock_queue(const dynamic_lock_queue& other) = delete;

      bool internal_full() const;
      size_t internal_capacity() const;

    public:

      // -1 : unbounded
      // 0 ... N : bounded (0 is silly)
      dynamic_lock_queue(int maxSize = kSmallDefault);

      bool lock_free() const;
      bool push(T& item);
      bool pop(T& popped_item);
      bool wait_and_pop(T& popped_item, std::chrono::milliseconds max_wait);
      bool full();
      bool empty() const;
      size_t size() const;
      size_t capacity() const;
      size_t capacity_free() const;
      size_t usage() const;

   };



   // maxSize of -1 equals unlimited size
   template<typename T>
   dynamic_lock_queue<T>::dynamic_lock_queue(int maxSize)
      : kMaxSize (maxSize){}

   template<typename T>
   bool dynamic_lock_queue<T>::lock_free() const {
      return false;
   }

   template<typename T>
   bool dynamic_lock_queue<T>::push(T& item) {
      {
         std::lock_guard<std::mutex> lock(m_);
         if (internal_full()) {
            return false;
         }
         queue_.push(std::move(item));
      } // lock_guard off
      data_cond_.notify_one();
      return true;
   }


   template<typename T>
   bool dynamic_lock_queue<T>::pop(T& popped_item) {
      std::lock_guard<std::mutex> lock(m_);
      if (queue_.empty()) {
         return false;
      }
      popped_item = std::move(queue_.front());
      queue_.pop();
      return true;
   }

   template<typename T>
   bool dynamic_lock_queue<T>::wait_and_pop(T& popped_item, std::chrono::milliseconds max_wait) {
      std::unique_lock<std::mutex> lock(m_);
      auto const timeout = std::chrono::steady_clock::now() + max_wait;
      while (queue_.empty()) {
         if (data_cond_.wait_until(lock, timeout) == std::cv_status::timeout) {
            break;
         }
         //  This 'while' loop is equal to
         //  data_cond_.wait(lock, [](bool result){return !queue_.empty();});
      }
      if (queue_.empty()) {
         return false;
      }

      popped_item = std::move(queue_.front());
      queue_.pop();
      return true;
   }

   template<typename T>
   bool dynamic_lock_queue<T>::full() {
      std::lock_guard<std::mutex> lock(m_);
      return internal_full();
   }

   template<typename T>
   bool dynamic_lock_queue<T>::empty() const {
      std::lock_guard<std::mutex> lock(m_);
      return queue_.empty();
   }

   template<typename T>
   size_t dynamic_lock_queue<T>::size() const {
      std::lock_guard<std::mutex> lock(m_);
      return queue_.size();
   }

   template<typename T>
   size_t dynamic_lock_queue<T>::capacity() const {
      std::lock_guard<std::mutex> lock(m_);
      return internal_capacity();
   }

   template<typename T>
   size_t dynamic_lock_queue<T>::capacity_free() const {
      std::lock_guard<std::mutex> lock(m_);
      return internal_capacity() - queue_.size();
   }

   template<typename T>
   size_t dynamic_lock_queue<T>::usage() const {
      std::lock_guard<std::mutex> lock(m_);
      return (100 * queue_.size() / internal_capacity());
   }

   // private
   template<typename T>
   size_t dynamic_lock_queue<T>::internal_capacity() const {
      if (kMaxSize == kUnlimited) {
         return std::numeric_limits<unsigned int>::max();
      }
      return kMaxSize;
   }


   template<typename T>
   bool dynamic_lock_queue<T>::internal_full() const {
      if (kMaxSize == kUnlimited) {
         return false;
      }
      return (queue_.size() >= kMaxSize);
   }
} // mpmc

