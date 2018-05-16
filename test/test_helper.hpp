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

namespace test_helper {
   // inspired by KjellK
   struct StopWatch {
      using seconds = std::chrono::seconds;
      using milliseconds = std::chrono::milliseconds;
      using nanoseconds = std::chrono::nanoseconds;
      using clock = std::chrono::steady_clock;
      clock::time_point start_;

      StopWatch() : start_(clock::now())
      {}
      ~StopWatch() = default;

      uint64_t ElapsedSec() const {
         return std::chrono::duration_cast<seconds>(clock::now() - start_).count();
      }
      uint64_t ElapsedMs() const {
         return std::chrono::duration_cast<milliseconds>(clock::now() - start_).count();
      }
      uint64_t ElapsedNs() const {
         return std::chrono::duration_cast<nanoseconds>(clock::now() - start_).count();
      }
   };
}