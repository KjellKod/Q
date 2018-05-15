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

#include <vector>
#include <utility>

// MPSC : Many Single Producers - Single Consumer
namespace mpsc {
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
   class roundrobin_receiver {
    public:
      roundrobin_receiver(std::vector<queue_api::Receiver<QType>> receivers);
      virtual ~roundrobin_receiver() = default;

      //template<typename Element>
      //bool pop(Element& item)

      template<typename Element>
      bool pop(Element& item) {
         bool result = false;
         const size_t loop_check = receivers_.size();

         size_t count = 0;
         while (!result && count++ < loop_check) {
            result = receivers_[current_].pop(item);
            current_ = increment(current_);
         }
         return result;
      }

      size_t increment(size_t idx) const;
      bool empty() const;
      bool full() const;
      size_t capacity() const;
      size_t capacity_free() const;
      size_t usage() const;
      size_t size() const;
      bool lock_free() const;

    private:
      std::vector<queue_api::Receiver<QType>> receivers_;
      size_t current_;
   };


   template<typename QType>
   roundrobin_receiver<QType>::roundrobin_receiver(std::vector<queue_api::Receiver<QType>> receivers)
      : receivers_(receivers)
      , current_(0) {

   }


   // template<typename Element>
   // bool roundrobin_receiver<QType>::pop(Element& item) {
   //    bool result = false;
   //    const size_t loop_check = receivers_.size();

   //    size_t count = 0;
   //    while (!result && count++ < loop_check) {
   //       result = receivers_[current_].pop(item);
   //       current_ = increment(current_);
   //    }
   //    return result;
   // }

   template<typename QType>
   size_t roundrobin_receiver<QType>::increment(size_t idx) const 
   { 
      return (idx + 1) % receivers_.size(); 
   }

   template<typename QType>
   bool roundrobin_receiver<QType>::empty() const {
      bool isempty = true;
      for (auto r : receivers_) {
         isempty = isempty && r.empty();
      }
      return isempty;
   }

   template<typename QType>
   bool roundrobin_receiver<QType>::full() const {
      bool isfull = true;
      for (auto r : receivers_) {
         isfull = isfull && r.full();
      }
      return isfull;
   }

   template<typename QType>
   size_t roundrobin_receiver<QType>::capacity() const {
      return receivers_.capacity() * receivers_.size();
   }

   template<typename QType>
   size_t roundrobin_receiver<QType>::capacity_free() const {
      size_t empty = 0;
      for (auto r : receivers_) {
         empty += r.capacity_free();
      }
      return empty;
   }

   template<typename QType>
   size_t roundrobin_receiver<QType>::usage() const {
      size_t percent_used = 0;
      for (auto r : receivers_) {
         percent_used += r.usage();
      }
      return (percent_used / receivers_.size());
   }

   template<typename QType>
   size_t roundrobin_receiver<QType>::size() const {
      size_t used = 0;
      for (auto r : receivers_) {
         used += r.size();
      }
      return used;
   }


   template<typename QType>
   bool roundrobin_receiver<QType>::lock_free() const {
      return receivers_[0].lock_free();
   }


} // mpsc



