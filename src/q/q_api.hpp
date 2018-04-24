/* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* Code & platform dependent issues with it was originally
* published at http://www.kjellkod.cc/threadsafecircularBase
* 2012-16-19  @author Kjell Hedström, hedstrom@kjellkod.cc
*
* Modified and inspired from KjellKod's code at:
* https://github.com/KjellKod/lock-free-wait-free-circularfifo
*/

#pragma once
#include <tuple>
#include <memory>

namespace queue_api {
// Base Queue API without pop() and push()
// It should be mentioned the thinking of what goes where
// it is a "controversy" whether what is tail and what is head
// http://en.wikipedia.org/wiki/FIFO#Head_or_tail_first
// This implementation follows "pop on head", "push on tail"
   template<typename QType>
   struct Base {
      Base(std::shared_ptr<QType> q)
         : _q(q)
         , _qref(*(q.get())) {
      }

      bool empty() const { return _qref.empty();}
      bool full() const { return _qref.full(); }
      size_t capacity() const { return _qref.capacity(); }
      size_t capacity_free() const { return _qref.capacity_free(); }
      size_t size() const { return _qref.size(); }
      bool lock_free() const { return _qref.size(); }
      size_t usage() const { return _qref.usage(); }

      std::shared_ptr<QType> _q;
      QType& _qref;
   };



// struct with: push() + base Queue API
   template<typename QType>
   struct Sender : public Base<QType> {
    public:
      Sender(std::shared_ptr<QType> q): Base<QType>(q) {}
      virtual ~Sender() = default;

      template<typename Element>
      bool push(Element& item) { return Base<QType>::_qref.push(item); }
   }; 



// struct with : pop() + base Queue API
   template<typename QType>
   struct Receiver : public Base<QType> {
    public:
      Receiver(std::shared_ptr<QType> q) :  Base<QType>(q) {}
      virtual ~Receiver() = default;

      template<typename Element>
      bool pop(Element& item) { return Base<QType>::_qref.pop(item); }
   };  // ReceiverQ



   template<typename QType, typename... Args>
   std::pair<Sender<QType>, Receiver<QType>> CreateQueue(Args&& ... args) {
      std::shared_ptr<QType> ptr = std::make_shared<QType>(std::forward< Args >(args)...);
      return std::make_pair(Sender<QType> {ptr}, Receiver<QType> {ptr});
   }

   enum index {sender = 0, receiver = 1};
} // QueueAPI








