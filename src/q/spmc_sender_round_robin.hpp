#pragma once


/*
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
* First published at: github.com/kjellkod/Q
*/

/*
MPSC - Multiple Producers - Sincle Consumer.
The queue consists of a set of SPSC queues.
Each (P)roducer has a dedicated queue
The (C)onsumer pops the queues in a round-robin manner.

IMPORTANT:
1. It is a "fair" scheduling in that each queue gets the same opportunity to push an item through.
2. Each queue has FIFO guarantees.
3. Between the queues there is no FIFO guarantee
4. A producer SPSC queue that is congested will have items that takes longer time to go through than a SPSC queue that is not congested. The Consumer pops each queue in a round-robin manner.
5. If there is no item available in the 'current' queue the POP(..) attempt will go to the next
   queue until at most all queues are visited once.
*/

#include "q/spsc_flexible_circular_fifo.hpp"
#include "q/q_api.hpp"
#include "q/round_robin_api.hpp"
#include <chrono>
#include <vector>
#include <utility>

// MPSC : Many Single Producers - Single Consumer
namespace spmc {
   namespace round_robin {
      // Use case: Many producers, one consumer.
      // Instead of using a MPMC queue you can use the MPSC
      //                          (Many Single Producer - One Single Consumer) queue.
      //
      // The Consumer will round-robin fetch attempts in a fair scheduling policy
      // the lock-free base type will typcially make this a much faster choice than using the mutex
      // protected MPSC
      //
      // WARNING: The same constraints as SPSC are in place for this queue. Only ONE thread may
      // act as the consumer
      template<typename QType>
      class Sender : public ::round_robin::API<QType, queue_api::Sender<QType>> {
       public:

         using QueueAPI = ::round_robin::API<QType, queue_api::Sender<QType>>;

         Sender(std::vector<queue_api::Sender<QType>> senders);
         virtual ~Sender() = default;

         template<typename Element>
         bool push(Element& item);
      };


      template<typename QType>
      Sender<QType>::Sender(std::vector<queue_api::Sender<QType>> senders)
         : QueueAPI(senders)
      {}

      template<typename QType>
      template<typename Element>
      bool Sender<QType>::push(Element& item) {
         bool result = false;
         const size_t loop_check = QueueAPI::queues_.size();

         size_t count = 0;
         while (!result && count++ < loop_check) {
            result = QueueAPI::queues_[QueueAPI::current_].push(item);
            QueueAPI::current_ = QueueAPI::increment(QueueAPI::current_);
         }
         return result;
      }
   } // round_robin
} // spmc
