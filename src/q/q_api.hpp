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
#include <chrono>

namespace queue_api {

   // std::declval converts any type T to a reference type,
   // making it possible to use member functions in decltype expressions without
   // construction the object http://en.cppreference.com/w/cpp/utility/declval
   //
   // std::decltype returns the reference type (used with declaval)
   // http://en.cppreference.com/w/cpp/language/decltype
   // namespace sfinae { // Substitution Failure Is Not An Error
   //    template <typename T, typename Element>
   //    struct wait_and_pop { // Detect if the class has function wait_and_pop(Element&, milliseconds)
   //       using Yes =  char;
   //       using No = char[2];
   //       static Element arg1;
   //       static std::chrono::milliseconds arg2;

   //       template<typename C, typename Arg> static auto verify(Arg arg, std::chrono::milliseconds wait)
   //       -> decltype(bool{std::declval<C>().wait_and_pop(arg, wait)}, Yes{});

   //       template<typename> static No& verify(...);

   //       // 'value' is compiled time calculated. If it fits the
   //       // 'wait_and_pop' signature 'value' is evaluated to true
   //       static const bool value = sizeof(verify<T>(arg1, arg2)) == sizeof(Yes);
   //    };
   // } // sfinae




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




   namespace sfinae {
      template <typename T, typename Element>
      bool wrapper(T& t, Element& e, std::chrono::milliseconds ms) {
         std::cout << "wrapper hit" << std::endl;
         return t.pop(e);
      }

      template <typename T, typename Element>
      auto match_call(T& t, Element& e, std::chrono::milliseconds ms, int) -> decltype( t.wait_and_pop(e, ms) )
      { return t.wait_and_pop(e, ms); }

      template <typename T, typename Element>
      auto match_call(T& t, Element& e, std::chrono::milliseconds ms, long) -> decltype( wrapper(t, e, ms) )
      { return wrapper(t, e, ms); }

      template <typename T, typename Element>
      int wait_and_pop (T& t, Element& e, std::chrono::milliseconds ms) {
         // SFINAE magic happens with the '0'.  For the right call it will be deducted ot bhe
         return match_call(t, e, ms, 0);
      }
   }




// struct with : pop() + base Queue API
   template<typename QType>
   struct Receiver : public Base<QType> {
    public:
      Receiver(std::shared_ptr<QType> q) :  Base<QType>(q) {}
      virtual ~Receiver() = default;

      template<typename Element>
      bool pop(Element& item) { return Base<QType>::_qref.pop(item); }

      template<typename Element>
      bool wait_and_pop(Element& item, const std::chrono::milliseconds wait_ms) {
         return sfinae::wait_and_pop(Base<QType>::_qref, item, wait_ms);
      }
      //    using milliseconds = std::chrono::milliseconds;
      //    using clock = std::chrono::steady_clock;
      //    using namespace std::chrono_literals;

      //    if (sfinae::wait_and_pop<QType, Element>::value) {
      //       return Base<QType>::_qref.wait_and_pop(item, wait_ms);
      //    } else {
      //       // return function-call with home rolled wait_and_pop
      //       auto t1 = clock::now();
      //       while (false == Base<QType>::_qref.pop(item)) {
      //          std::this_thread::sleep_for(50ns);
      //          auto elapsed_ms = std::chrono::duration_cast<milliseconds>(clock::now() - t1);//.count();
      //          if (elapsed_ms > wait_ms) {
      //             return false;
      //          }
      //       }
      //       return true;
      //    }
   };




   template<typename QType, typename... Args>
   std::pair<Sender<QType>, Receiver<QType>> CreateQueue(Args&& ... args) {
      std::shared_ptr<QType> ptr = std::make_shared<QType>(std::forward< Args >(args)...);
      return std::make_pair(Sender<QType> {ptr}, Receiver<QType> {ptr});
   }

   enum index {sender = 0, receiver = 1};
} // queue_api








