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
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "q/q_api.hpp"
#include "stopwatch.hpp"

#define Q_CHECK_EQ(value1, value2)                                                \
   do {                                                                           \
      if ((value1) != (value2)) {                                                 \
         std::cout << "Line " << __LINE__ << ": CHECK failed, "                   \
                   << (value1) << " is not equal with " << (value2) << std::endl; \
         assert((value1) == (value2));                                            \
      }                                                                           \
   } while (0)

#define Q_CHECK(expr)                                           \
   do {                                                         \
      if (!(expr)) {                                            \
         std::cout << "Line " << __LINE__ << ": CHECK failed, " \
                   << #expr << " is not true" << std::endl;     \
         assert((expr));                                        \
      }                                                         \
   } while (0)

namespace benchmark {
   struct result_t {
      uint64_t total_sum;
      uint64_t elapsed_time_in_ns;
   };

   const std::chrono::milliseconds kMaxWaitMs(1000);

   template <typename Sender>
   result_t Push(Sender q, const size_t stop, std::atomic<bool>& producerStart, std::atomic<bool>& consumerStart) {
      using namespace std::chrono_literals;
      producerStart.store(true);
      while (!consumerStart.load()) {
         std::this_thread::sleep_for(10ns);
      }
      benchmark::stopwatch watch;
      uint64_t sum = 0;
      for (unsigned int i = 1; i <= stop; ++i) {
         Q_CHECK(q.wait_and_push(i, kMaxWaitMs));
         sum += i;
      }
      return {sum, watch.elapsed_ns()};
   }

   template <typename Receiver>
   result_t Get(Receiver q, size_t stop, std::atomic<bool>& producerStart, std::atomic<bool>& consumerStart) {
      using namespace std::chrono_literals;
      consumerStart.store(true);
      while (!producerStart.load()) {
         std::this_thread::sleep_for(10ns);
      }
      benchmark::stopwatch watch;
      uint64_t sum = 0;
      for (;;) {
         unsigned int value = 0;
         Q_CHECK(q.wait_and_pop(value, kMaxWaitMs));
         sum += value;
         if (value == stop) {
            break;
         }
      }
      return {sum, watch.elapsed_ns()};
   }
}  // namespace benchmark

//    template <typename Sender>
//    size_t PushUntil(Sender q, std::string data,
//                     std::atomic<size_t>& producerCount,
//                     std::atomic<bool>& stopRunning) {
//       using namespace std::chrono_literals;
//       producerCount++;
//       using namespace std::chrono_literals;

//       benchmark::stopwatch watch;
//       size_t amountPushed = 0;
//       while (!stopRunning.load(std::memory_order_relaxed)) {
//          std::string value = data;
//          while (false == q.push(value) && !stopRunning.load(std::memory_order_relaxed)) {
//             std::this_thread::sleep_for(100ns); // yield is too aggressive
//          }
//          ++amountPushed;
//       }
//       return amountPushed;
//    }

//    //    template <typename Receiver>
//    //    size_t GetUntil(Receiver q, const std::string data,
//    //                    std::atomic<size_t>& consumerCount,
//    //                    std::atomic<bool>& stopRunning) {
//    //       using namespace std::chrono_literals;
//    //       consumerCount++;

//    //       benchmark::stopwatch watch;
//    //       size_t amountReceived = 0;
//    //       size_t byteReceived = 0;
//    //       while (!stopRunning.load(std::memory_order_relaxed)) {
//    //          std::string value;
//    //          bool result = false;
//    //          std::chrono::milliseconds wait{10};
//    //          while (!(result = q.wait_and_pop(value, wait))) {
//    //             if (stopRunning.load(std::memory_order_relaxed)) {
//    //                break;
//    //             }
//    //          }
//    //          if (result) {
//    //             EXPECT_EQ(data.size(), value.size());
//    //             EXPECT_FALSE(value.was_empty());
//    //             ++amountReceived;
//    //             byteReceived += value.size();
//    //          }
//    //       }
//    //       std::ostringstream oss;
//    //       oss << "Bytes received: " << byteReceived << std::endl;
//    //       std::cout << oss.str();
//    //       return amountReceived;
//    //    }