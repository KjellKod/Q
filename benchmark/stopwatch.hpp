/*
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* Originally published at https://github.com/KjellKod/Q
*/

#pragma once
#include <chrono>

namespace benchmark {

   // inspired by github/KjellKod/StopWatch
   struct stopwatch {
      using seconds = std::chrono::seconds;
      using milliseconds = std::chrono::milliseconds;
      using microseconds = std::chrono::microseconds;
      using nanoseconds = std::chrono::nanoseconds;
      using clock = std::chrono::steady_clock;
      clock::time_point start_;

      stopwatch() :
          start_(std::chrono::high_resolution_clock::now()) {}
      ~stopwatch() = default;

      auto now() const {
         return std::chrono::high_resolution_clock::now();
      };

      uint64_t elapsed_sec() const {
         return std::chrono::duration_cast<seconds>(now() - start_).count();
      }
      uint64_t elapsed_ms() const {
         return std::chrono::duration_cast<milliseconds>(now() - start_).count();
      }
      uint64_t elapsed_us() const {
         return std::chrono::duration_cast<microseconds>(now() - start_).count();
      }
      uint64_t elapsed_ns() const {
         return std::chrono::duration_cast<nanoseconds>(now() - start_).count();
      }
   };

}  // namespace benchmark