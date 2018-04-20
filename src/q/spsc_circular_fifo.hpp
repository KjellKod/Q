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
* 2012-16-19  @author Kjell Hedström, hedstrom@kjellkod.cc
*
* Modified from KjellKod's code at:
* https://github.com/KjellKod/lock-free-wait-free-circularfifo
*/

// should be mentioned the thinking of what goes where
// it is a "controversy" whether what is tail and what is head
// http://en.wikipedia.org/wiki/FIFO#Head_or_tail_first

#pragma once

#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

namespace spsc {
   template<typename Element>
   class circular_fifo {
    public:

      circular_fifo(const size_t size)
         : kSize(size)
         , kCapacity(kSize + 1)
         , _tail(0)
         , _head(0) {
           _array.reserve(kCapacity);
         }

      virtual ~circular_fifo() {}

      bool push(const Element& item);
      bool pop(Element& item);

      bool is_empty() const;
      bool is_full() const;
      bool is_lock_free() const;

      unsigned size() const { return 1; }


    private:
      typedef char cache_line[64];
      size_t increment(size_t idx) const { return (idx + 1) % kCapacity; }
      const size_t kSize;
      const size_t kCapacity;

      cache_line _pad_storage;
      std::vector<Element> _array;

      cache_line _pad_tail; 
      std::atomic <size_t>  _tail;
      cache_line  _pad_head;
      std::atomic<size_t>   _head; // head(output) index
   };


   template<typename Element>
   bool circular_fifo<Element>::push(const Element& item) {
      const auto current_tail = _tail.load(std::memory_order_relaxed);
      const auto next_tail = increment(current_tail);
      if (next_tail != _head.load(std::memory_order_acquire)) {
         _array[current_tail] = item;
         _tail.store(next_tail, std::memory_order_release);
         return true;
      }

      return false; // full queue
   }


// Pop by Consumer can only update the head (load with relaxed, store with release)
//     the tail must be accessed with at least aquire
   template<typename Element>
   bool circular_fifo<Element>::pop(Element& item) {
      const auto current_head = _head.load(std::memory_order_relaxed);
      if (current_head == _tail.load(std::memory_order_acquire))
         return false; // empty queue

      item = _array[current_head];
      _head.store(increment(current_head), std::memory_order_release);
      return true;
   }

   template<typename Element>
   bool circular_fifo<Element>::is_empty() const {
      // snapshot with acceptance of that this comparison operation is not atomic
      return (_head.load(std::memory_order_relaxed) == _tail.load(std::memory_order_relaxed));
   }


// snapshot with acceptance that this comparison is not atomic
   template<typename Element>
   bool circular_fifo<Element>::is_full() const {
      const auto next_tail = increment(_tail.load(std::memory_order_relaxed)); // aquire, we dont know who call
      return (next_tail == _head.load(std::memory_order_relaxed));
   }


   template<typename Element>
   bool circular_fifo<Element>::is_lock_free() const {
      return (_tail.is_lock_free() && _head.is_lock_free());
   }

} // spsc
