#pragma once

#include <chrono>
#include <thread>

namespace sfinae_sender {
   // SFINAE: Substitution Failure Is Not An Error
   // Decide at compile time what function signature to use
   // 1. If 'wait_and_pop' exists in the queue it uses that
   // 2. If only 'pop' exists it implements 'wait_and_pop' expected
   // -- FYI: The wait is set to increments of 100 ns
   template <typename T, typename Element>
   bool wrapper(T& t, Element& e, std::chrono::milliseconds max_wait) {
      using milliseconds = std::chrono::milliseconds;
      using clock = std::chrono::steady_clock;
      using namespace std::chrono_literals;
      auto t1 = clock::now();
      bool result = false;
      while (!(result = t.push(e))) {
         std::this_thread::sleep_for(100ns);
         auto elapsed_ms = std::chrono::duration_cast<milliseconds>(clock::now() - t1);
         if (elapsed_ms > max_wait) {
            return result;
         }
      }
      return result;
   }

   template <typename T, typename Element>
   auto match_call(T& t, Element& e, std::chrono::milliseconds ms, int) -> decltype(t.wait_and_push(e, ms)) {
      return t.wait_and_push(e, ms);
   }

   template <typename T, typename Element>
   auto match_call(T& t, Element& e, std::chrono::milliseconds ms, long) -> decltype(wrapper(t, e, ms)) {
      return wrapper(t, e, ms);
   }

   template <typename T, typename Element>
   int wait_and_push(T& t, Element& e, std::chrono::milliseconds ms) {
      // SFINAE magic happens with the '0'.
      // For the matching call the '0' will be typed to int.
      // For non-matching call it will be typed to long
      return match_call(t, e, ms, 0);
   }

}  // namespace sfinae_sender