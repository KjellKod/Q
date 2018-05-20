/*
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
* First published at: github.com/kjellkod/Q
* 
* MPSC - Multiple Producers - Sincle Consumer.
* SPMC - One Producer - Multiple Consumers.
* 
* The 'unified' side has access to many queues - each a SPSC queue
* The 'multiple' sub-part are each accessing a SPSC queue
* It's vital that only one thread touches the end point of a SPSC queue
* 
* IMPORTANT:
* 1. It is a "fair" scheduling in that each queue gets the same opportunity to push an item through.
* 2. Each queue has FIFO guarantees.
* 3. Between the queues there is no FIFO guarantee
* 4. A producer SPSC queue that is congested will have items that takes longer time to go through than a SPSC queue 
*    that is not congested. The Consumer pops each queue in a round-robin manner.
* 5. If there is no item available in the 'current' queue the POP(..) attempt will go to the next
*    queue until at most all queues are visited once.
*/

#pragma once 

#include "q/q_api.hpp"
#include <vector>
#include <utility>

namespace round_robin {
   // Use case: Many producers, one consumer.(each with dedicated queue)
   // Use case: One producers, many consumer(each with dedicated queue)
   //
   // WARNING: The same constraints as SPSC are in place for the round robin queues.
   // Only ONE thread may touch each queue's end API (receiver side, or sender side)
   template<typename QType, typename QueueUsageApi>
   class API {
    public:
      API(std::vector<QueueUsageApi> queues);
      virtual ~API() = default;

      size_t increment(size_t idx) const;
      bool empty() const;
      bool full() const;
      size_t capacity() const;
      size_t capacity_free() const;
      size_t usage() const;
      size_t size() const;
      bool lock_free() const;


    protected:
      std::vector<QueueUsageApi> queues_;
      size_t current_;
   };


   template<typename QType, typename QueueUsageApi>
   API<QType, QueueUsageApi>::API(std::vector<QueueUsageApi> queues)
      : queues_(queues)
      , current_(0) {

   }

   template<typename QType, typename QueueUsageApi>
   size_t API<QType, QueueUsageApi>::increment(size_t idx) const {
      return (idx + 1) % queues_.size();
   }

   template<typename QType, typename QueueUsageApi>
   bool API<QType, QueueUsageApi>::empty() const {
      bool isempty = true;
      for (auto r : queues_) {
         isempty = isempty && r.empty();
      }
      return isempty;
   }

   template<typename QType, typename QueueUsageApi>
   bool API<QType, QueueUsageApi>::full() const {
      bool isfull = true;
      for (auto r : queues_) {
         isfull = isfull && r.full();
      }
      return isfull;
   }

   template<typename QType, typename QueueUsageApi>
   size_t API<QType, QueueUsageApi>::capacity() const {
      size_t max = 0;
      for (auto r : queues_) {
         max += r.capacity();
      }
      return max;
   }

   template<typename QType, typename QueueUsageApi>
   size_t API<QType, QueueUsageApi>::capacity_free() const {
      size_t empty = 0;
      for (auto r : queues_) {
         empty += r.capacity_free();
      }
      return empty;
   }

   template<typename QType, typename QueueUsageApi>
   size_t API<QType, QueueUsageApi>::usage() const {
      size_t percent_used = 0;
      for (auto r : queues_) {
         percent_used += r.usage();
      }
      return (percent_used / queues_.size());
   }

   template<typename QType, typename QueueUsageApi>
   size_t API<QType, QueueUsageApi>::size() const {
      size_t used = 0;
      for (auto r : queues_) {
         used += r.size();
      }
      return used;
   }


   template<typename QType, typename QueueUsageApi>
   bool API<QType, QueueUsageApi>::lock_free() const {
      bool lockless = true;
      for (auto r : queues_) {
         lockless = lockless && r.lock_free();
      }
      return lockless;
   }
} // round_robin?API
