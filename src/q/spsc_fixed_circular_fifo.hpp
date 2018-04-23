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
// http://en.wikipedia.org/wiki/FIFO#Head_or_tail_first

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


         circular_fifo() : _tail(0), _head(0) {}
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
         size_t tail() const { return _tail.load(); }
         size_t head() const { return _head.load(); }



       private:
         size_t increment(size_t idx) const { return (idx + 1) % kCapacity; }

         cache_line _pad_storage;
         alignas(64) Element _array[kCapacity];
         cache_line _pad_tail;
         alignas(64) std::atomic <size_t>  _tail;
         cache_line  _pad_head;
         alignas(64) std::atomic<size_t>   _head; // head(output) index
      };


      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::push(Element& item) {
         const auto current_tail = _tail.load(std::memory_order_relaxed);
         const auto next_tail = increment(current_tail);
         if (next_tail != _head.load(std::memory_order_acquire)) {
            _array[current_tail] = std::move(item);
            _tail.store(next_tail, std::memory_order_release);
            return true;
         }

         return false; // full queue

      }


// Pop by Consumer can only update the head (load with relaxed, store with release)
//     the tail must be accessed with at least aquire
      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::pop(Element& item) {
         const auto current_head = _head.load(std::memory_order_relaxed);
         if (current_head == _tail.load(std::memory_order_acquire))
            return false; // empty queue

         item = std::move(_array[current_head]);
         _head.store(increment(current_head), std::memory_order_release);
         return true;
      }

      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::empty() const {
         // snapshot with acceptance of that this comparison operation is not atomic
         return (_head.load(std::memory_order_relaxed) == _tail.load(std::memory_order_relaxed));
      }


// snapshot with acceptance that this comparison is not atomic
      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::full() const {
         const auto next_tail = increment(_tail.load(std::memory_order_relaxed)); // aquire, we dont know who call
         return (next_tail == _head.load(std::memory_order_relaxed));
      }


      template<typename Element, size_t Size>
      bool circular_fifo<Element, Size>::lock_free() const {
         return (_tail.is_lock_free() && _head.is_lock_free());
      }

      template<typename Element, size_t Size>
      size_t circular_fifo<Element, Size>::size() const {
         return ((_tail.load() - _head.load() + kCapacity) % kCapacity);
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
