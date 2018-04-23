/* Not any company's property but Public-Domain
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
* Modified and inspired from KjellKod's code at:
* https://github.com/KjellKod/lock-free-wait-free-circularfifo
*/

#pragma once
#include <tuple>
#include <memory>


// Base Queue API without pop() and push()
// It should be mentioned the thinking of what goes where
// it is a "controversy" whether what is tail and what is head
// http://en.wikipedia.org/wiki/FIFO#Head_or_tail_first
//
// This implementation follows "pop on head", "push on tail"
template<typename Type>
struct Queue {
   enum {ProducerIndex = 0, ConsumerIndex = 1};

   Queue(std::shared_ptr<Type> q)
      : _q(q)
      , _qref(*(q.get())) {
   }

   bool empty() const { return _qref.empty();}
   bool full() const { return _qref.full(); }
   size_t capacity() const { return _qref.capacity(); }
   size_t capacity_free() const { return _qref.capacity_free(); }
   size_t size() const { return _qref.size(); }
   bool lock_free() const { return _qref.size(); }
   size_t tail() const { return _qref.tail(); }
   size_t head() const { return _qref.head(); }
   size_t usage() const { return _qref.usage(); }

   std::shared_ptr<Type> _q;
   Type& _qref;
};

// Producer: push() + base Queue API
template<typename Type>
struct Producer : public Queue<Type> {
 public:
   Producer(std::shared_ptr<Type> q): Queue<Type>(q) {}
   //: Queue<Type>::_q(q)
   //, Queue<Type>::_qref(*(q.get())) {
//}
virtual ~Producer() = default;

template<typename Element>
bool push(const Element& item) { return Queue<Type>::_qref.push(item); }

}; // Producer



// Consumer: pop() + base Queue API
template<typename Type>
struct Consumer : public Queue<Type> {
 public:
   Consumer(std::shared_ptr<Type> q) : Queue<Type>(q) {}
   // : Queue<Type>::_q(q)
   // , Queue<Type>::_qref(*(q.get())) {
//   }

   virtual ~Consumer() = default;

   template<typename Element>
   bool pop(Element& item) { return Queue<Type>::_qref.pop(item); }
};  // Consumer


template<typename Type, typename... Args>
std::pair<Producer<Type>, Consumer<Type>> CreateQueue(Args&& ... args) {
   std::shared_ptr<Type> ptr = std::make_shared<Type>(std::forward< Args >(args)...);
   return std::make_pair(Producer<Type> {ptr}, Consumer<Type> {ptr});
}








