/*
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* Code & platform dependent issues with it was originally
* published at http://www.kjellkod.cc/threadsafecircularqueue
* 2012-16-19  @author Kjell Hedström, hedstrom@kjellkod.cc */

// should be mentioned the thinking of what goes where
// it is a "controversy" whether what is tail and what is head
// http://en.wikipedia.org/wiki/FIFO#Head_ortail__first

#pragma once

#include <atomic>
#include <cstddef>
#include <thread>

namespace spsc {
   namespace fixed {
      template<typename Element, size_t Size>
      class circular_fifo {
       public:

         typedef char cache_line[64];
         enum alignas(64) { kSize = Size };
         enum alignas(64) { kCapacity = kSize + 1 };       // http://en.cppreference.com/w/cpp/types/aligned_storage


         circular_fifo() : tail_(0), head_(0) {}
         virtual ~circular_fifo() {}

         bool push(Element& item);
         bool pop(Element& item);
         bool empty() const;
         bool full() const;
         size_t capacity() const;
         size_t capacity_free() const;
         size_t usage() const;
         size_t size() const;
         bool lock_free() const;
         size_t tail() const { return tail_.load(); }
         size_t head() const { return head_.load(); }



       private:
         size_t increment(size_t idx) const { return (idx + 1) % kCapacity; }

         cache_line pad_storage_;
         alignas(64) Element array_[kCapacity];
         cache_line padtail_;
         alignas(64) std::atomic <size_t>  tail_;
         cache_line  padhead_;
         alignas(64) std::atomic<size_t>   head_; // head(output) index
         cache_line padend_;

      };


      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::push(Element& item) {
         const auto currenttail_ = tail_.load(std::memory_order_relaxed);
         const auto nexttail_ = increment(currenttail_);
         if (nexttail_ != head_.load(std::memory_order_acquire)) {
            array_[currenttail_] = std::move(item);
            tail_.store(nexttail_, std::memory_order_release);
            return true;
         }

         return false; // full queue

      }


// Pop by Consumer can only update the head (load with relaxed, store with release)
//     the tail must be accessed with at least aquire
      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::pop(Element& item) {
         const auto currenthead_ = head_.load(std::memory_order_relaxed);
         if (currenthead_ == tail_.load(std::memory_order_acquire))
            return false; // empty queue

         item = std::move(array_[currenthead_]);
         head_.store(increment(currenthead_), std::memory_order_release);
         return true;
      }

      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::empty() const {
         // snapshot with acceptance of that this comparison operation is not atomic
         return (head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed));
      }


// snapshot with acceptance that this comparison is not atomic
      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::full() const {
         const auto nexttail_ = increment(tail_.load(std::memory_order_relaxed)); // aquire, we dont know who call
         return (nexttail_ == head_.load(std::memory_order_relaxed));
      }


      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::lock_free() const {
         return std::atomic<size_t>{}.is_lock_free();
      }

      template<typename Element, size_t Size>
      size_t circular_fifo<Element, Size>::size() const {
         return ((tail_.load() - head_.load() + kCapacity) % kCapacity);
      }

      template<typename Element, size_t Size>
      size_t circular_fifo<Element, Size>::capacity_free() const {
         return (kCapacity - size() - 1);
      }

      template<typename Element, size_t Size>
      size_t circular_fifo<Element, Size>::capacity() const {
         return kSize;
      }


      template<typename Element, size_t Size>
      size_t circular_fifo<Element, Size>::usage() const {
         return (100 * size() / kSize);
      }
   } // fixed
} // spsc
