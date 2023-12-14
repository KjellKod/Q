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
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <vector>
#include "q/q_api.hpp"
#include "test/test_helper.hpp"

namespace test_performance {
   struct ResultType2 {
      uint64_t nanotime;
      uint64_t sum;
   };

   using namespace test_helper;
   const size_t kMaxWaitMs = 1000;

   template <typename Sender>
   ResultType Push(Sender q, const size_t stop, std::atomic<bool>& producerStart, std::atomic<bool>& consumerStart) {
      using namespace std::chrono_literals;
      producerStart.store(true);
      while (!consumerStart.load()) {
        std::this_thread::sleep_for(100ns);
      }

      StopWatch watch;
      uint64_t sum = 0;
      for (auto i = 1; i <= stop + 1; ++i) {
         q.wait_and_push(i, kMaxWaitMs);
         sum += i;
      }
      return {watch.ElapsedNs(), sum};
   }

   template <typename Receiver>
   ResultType Get(Receiver q, size_t stop, std::atomic<bool>& producerStart, std::atomic<bool>& consumerStart) {
      using namespace std::chrono_literals;
      consumerStart.store(true);
      while (!producerStart.load()) {
         std::this_thread::sleep_for(100ns);
      }

      StopWatch watch;
      uint64_t sum = 0;
      for (auto i = 1; i <= stop; ++i) {
         auto value = 0;
         assert(q.wait_and_pop(value, kMaxWaitMs));
         sum += value;
      }
      return {watch.ElapsedNs(), sum};
   }


//    template <typename T>
//    std::vector<ResultType> RunSPS2C(T queue, size_t howMany) {
//       std::atomic<bool> producerStart{false};
//       std::atomic<bool> consumerStart{false};
//       std::vector<ResultType> result; 

//       using namespace std;
//       using namespace chrono;
//       auto producer = std::get<queue_api::index::sender>(queue);
//       auto consumer = std::get<queue_api::index::receiver>(queue);

//       StopWatch watch; 
//       size_t start = 1;
//       size_t stop = howMany;
//       auto prodResult = std::async(std::launch::async, Push<decltype(producer)>,
//                                    producer, start, stop, std::ref(producerStart), std::ref(consumerStart));
//       auto consResult = std::async(std::launch::async, Get<decltype(consumer)>,
//                                    consumer, start, stop, std::ref(producerStart), std::ref(consumerStart));

//       auto sent = prodResult.get();
//       auto received = consResult.get();
//       ResultType expected = {watch.elapsedTimeNs(), 0 };
//       return {sent, received, expected}}


//     //   auto t2 = high_resolution_clock::now();
//     //   auto us = duration_cast<microseconds>(t2 - t1).count();
//     //   std::cout << "Push - Pull #" << howMany << " items in: " << us << " us" << std::endl;
//     //   std::cout << "Average: " << 1000 * ((float)us / (float)howMany) << " ns" << std::endl;
//    }





   //    template <typename Sender>
   //    size_t PushUntil(Sender q, std::string data,
   //                     std::atomic<size_t>& producerCount,
   //                     std::atomic<bool>& stopRunning) {
   //       using namespace std::chrono_literals;
   //       producerCount++;
   //       using namespace std::chrono_literals;

   //       StopWatch watch;
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

   //    template <typename Receiver>
   //    size_t GetUntil(Receiver q, const std::string data,
   //                    std::atomic<size_t>& consumerCount,
   //                    std::atomic<bool>& stopRunning) {
   //       using namespace std::chrono_literals;
   //       consumerCount++;

   //       StopWatch watch;
   //       size_t amountReceived = 0;
   //       size_t byteReceived = 0;
   //       while (!stopRunning.load(std::memory_order_relaxed)) {
   //          std::string value;
   //          bool result = false;
   //          std::chrono::milliseconds wait{10};
   //          while (!(result = q.wait_and_pop(value, wait))) {
   //             if (stopRunning.load(std::memory_order_relaxed)) {
   //                break;
   //             }
   //          }
   //          if (result) {
   //             EXPECT_EQ(data.size(), value.size());
   //             EXPECT_FALSE(value.empty());
   //             ++amountReceived;
   //             byteReceived += value.size();
   //          }
   //       }
   //       std::ostringstream oss;
   //       oss << "Bytes received: " << byteReceived << std::endl;
   //       std::cout << oss.str();
   //       return amountReceived;
   //    }