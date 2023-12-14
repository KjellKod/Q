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
#include <q/mpsc_receiver_round_robin.hpp>
#include <q/q_api.hpp>
#include <q/spmc_sender_round_robin.hpp>
#include <string>
#include <vector>
#include "test_helper.hpp"

namespace test_performance {
   using ResultType = std::vector<std::string>;
   using namespace test_helper;
   using sum_t = long long;

   template <typename Sender>
   ResultType Push(Sender q, size_t start, size_t stop, std::atomic<bool>& producerStart, std::atomic<bool>& consumerStart) {
      using namespace std::chrono_literals;
      std::vector<std::string> expected;
      expected.reserve(stop - start);
      producerStart.store(true);
      using namespace std::chrono_literals;
      while (!consumerStart.load()) {
         std::this_thread::sleep_for(1us);
      }

      for (auto i = start; i < stop; ++i) {
         std::string value = std::to_string(i);
         expected.push_back(value);
         while (false == q.push(value)) {
            std::this_thread::sleep_for(1us);  // // yield is too aggressive
         }
      }
      return expected;
   }

   template <typename Receiver>
   ResultType Get(Receiver q, size_t start, size_t stop, std::atomic<bool>& producerStart, std::atomic<bool>& consumerStart) {
      using namespace std::chrono_literals;
      std::vector<std::string> received;
      received.reserve(stop - start);
      consumerStart.store(true);
      while (!producerStart.load()) {
         std::this_thread::sleep_for(1us);
      }

      for (auto i = start; i < stop; ++i) {
         std::string value;
         while (false == q.pop(value)) {
            std::this_thread::sleep_for(1us);  // yield is too aggressive
         }
         received.push_back(value);
      }
      return received;
   }

   template <typename Sender>
   size_t PushUntil(Sender q, std::string data,
                    std::atomic<size_t>& producerCount,
                    std::atomic<bool>& stopRunning) {
      using namespace std::chrono_literals;
      producerCount++;
      using namespace std::chrono_literals;

      StopWatch watch;
      size_t amountPushed = 0;
      while (!stopRunning.load(std::memory_order_relaxed)) {
         std::string value = data;
         while (false == q.push(value) && !stopRunning.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(100ns);  // yield is too aggressive
         }
         ++amountPushed;
      }
      return amountPushed;
   }

   template <typename Receiver>
   size_t GetUntil(Receiver q, const std::string data,
                   std::atomic<size_t>& consumerCount,
                   std::atomic<bool>& stopRunning) {
      using namespace std::chrono_literals;
      consumerCount++;

      StopWatch watch;
      size_t amountReceived = 0;
      size_t byteReceived = 0;
      while (!stopRunning.load(std::memory_order_relaxed)) {
         std::string value;
         bool result = false;
         std::chrono::milliseconds wait{10};
         while (!(result = q.wait_and_pop(value, wait))) {
            if (stopRunning.load(std::memory_order_relaxed)) {
               break;
            }
         }
         if (result) {
            EXPECT_EQ(data.size(), value.size());
            EXPECT_FALSE(value.empty());
            ++amountReceived;
            byteReceived += value.size();
         }
      }
      std::ostringstream oss;
      oss << "Bytes received: " << byteReceived << std::endl;
      std::cout << oss.str();
      return amountReceived;
   }

   template <typename T>
   std::vector<ResultType> RunSPS2C(T queue, size_t howMany) {
      std::atomic<bool> producerStart{false};
      std::atomic<bool> consumerStart{false};
      std::vector<ResultType> result; 

      using namespace std;
      using namespace chrono;
      auto producer = std::get<queue_api::index::sender>(queue);
      auto consumer = std::get<queue_api::index::receiver>(queue);

      StopWatch watch; 
      size_t start = 1;
      size_t stop = howMany;
      auto prodResult = std::async(std::launch::async, Push<decltype(producer)>,
                                   producer, start, stop, std::ref(producerStart), std::ref(consumerStart));
      auto consResult = std::async(std::launch::async, Get<decltype(consumer)>,
                                   consumer, start, stop, std::ref(producerStart), std::ref(consumerStart));

      auto sent = prodResult.get();
      auto received = consResult.get();
      ResultType expected = {watch.ElapsedNs(), 0 };
      return {sent, received, expected};
    //   auto t2 = high_resolution_clock::now();
    //   auto us = duration_cast<microseconds>(t2 - t1).count();
    //   std::cout << "Push - Pull #" << howMany << " items in: " << us << " us" << std::endl;
    //   std::cout << "Average: " << 1000 * ((float)us / (float)howMany) << " ns" << std::endl;
   }


   template <typename T>
   void RunSPSC(T queue, size_t howMany) {
      std::atomic<bool> producerStart{false};
      std::atomic<bool> consumerStart{false};

      using namespace std;
      using namespace chrono;
      auto producer = std::get<queue_api::index::sender>(queue);
      auto consumer = std::get<queue_api::index::receiver>(queue);

      auto t1 = high_resolution_clock::now();
      size_t start = 1;
      size_t stop = howMany;
      auto prodResult = std::async(std::launch::async, Push<decltype(producer)>,
                                   producer, start, stop, std::ref(producerStart), std::ref(consumerStart));
      auto consResult = std::async(std::launch::async, Get<decltype(consumer)>,
                                   consumer, start, stop, std::ref(producerStart), std::ref(consumerStart));

      auto expected = prodResult.get();
      auto received = consResult.get();
      auto t2 = high_resolution_clock::now();
      auto us = duration_cast<microseconds>(t2 - t1).count();
      std::cout << "Push - Pull #" << howMany << " items in: " << us << " us" << std::endl;
      std::cout << "Average: " << 1000 * ((float)us / (float)howMany) << " ns" << std::endl;

      EXPECT_EQ(howMany, received.size());
      EXPECT_EQ(expected, received);
   }

   template <typename T>
   void RunMPMC(T queue, std::string data, size_t numberProducers,
                size_t numberConsumers, const size_t timeToRunInSec) {
      std::atomic<size_t> producerCount{0};
      std::atomic<size_t> consumerCount{0};
      std::atomic<bool> producerStop{false};
      std::atomic<bool> consumerStop{false};

      using namespace std;
      using namespace std::chrono;
      auto producer = std::get<queue_api::index::sender>(queue);
      auto consumer = std::get<queue_api::index::receiver>(queue);
      std::vector<std::future<size_t>> producerResult;
      producerResult.reserve(numberProducers);

      for (size_t i = 0; i < numberProducers; ++i) {
         producerResult.emplace_back(std::async(std::launch::async, PushUntil<decltype(producer)>,
                                                producer, data,
                                                std::ref(producerCount),
                                                std::ref(producerStop)));
      }
      std::vector<std::future<size_t>> consumerResult;
      consumerResult.reserve(numberConsumers);
      for (size_t i = 0; i < numberConsumers; ++i) {
         consumerResult.emplace_back(std::async(std::launch::async, GetUntil<decltype(consumer)>,
                                                consumer, data,
                                                std::ref(consumerCount),
                                                std::ref(consumerStop)));
      }

      using namespace std::chrono_literals;
      while (consumerCount.load() < numberConsumers && producerCount.load() < numberProducers) {
         std::this_thread::sleep_for(1us);
      }
      StopWatch elapsedRun;
      while (elapsedRun.ElapsedSec() < timeToRunInSec) {
         std::this_thread::sleep_for(1us);
      }

      producerStop.store(true, std::memory_order_release);
      size_t amountProduced = 0;
      for (auto& result : producerResult) {
         amountProduced += result.get();
      }
      consumerStop.store(true, std::memory_order_release);
      size_t amountConsumed = 0;
      for (auto& result : consumerResult) {
         amountConsumed += result.get();
      }

      // amoundProduced >= amountConsumed
      // amountProduced <= amountConsumed + 100
      EXPECT_GE(amountProduced, amountConsumed) << "produced: " << amountProduced
                                                << ", consumed: " << amountConsumed << ", capacity: " << producer.capacity();

      auto elapsedTimeNs = elapsedRun.ElapsedNs();
      auto elapsedTimeSec = elapsedTimeNs / (1000000000);
      std::cout << "Transaction/s: " << amountConsumed / elapsedTimeSec << std::endl;
      std::cout << "Average transaction: " << elapsedTimeNs / amountConsumed << " ns" << std::endl;
      std::cout << "Transaction/s per consumer: " << amountConsumed / elapsedTimeSec / numberConsumers << std::endl;
      std::cout << "Transation GByte/s: " << amountConsumed * data.size() / (1024 * 1024 * 1024) / elapsedTimeSec << std::endl;
   }

   template <typename QType, typename QTypePair>
   void RunMPSC(std::vector<QTypePair> queues, std::string data, const size_t timeToRunInSec) {
      std::atomic<size_t> producerCount{0};
      std::atomic<size_t> consumerCount{0};
      std::atomic<bool> producerStop{false};
      std::atomic<bool> consumerStop{false};

      std::vector<queue_api::Receiver<QType>> receivers;
      std::vector<queue_api::Sender<QType>> senders;

      for (auto& q : queues) {
         receivers.push_back(std::get<queue_api::index::receiver>(q));
         senders.push_back(std::get<queue_api::index::sender>(q));
      }
      const size_t numberProducers = senders.size();
      mpsc::round_robin::Receiver<QType> consumer(receivers);

      std::vector<std::future<size_t>> producerResult;
      producerResult.reserve(senders.size());

      for (size_t i = 0; i < senders.size(); ++i) {
         auto producer = senders[i];
         producerResult.emplace_back(std::async(std::launch::async, PushUntil<decltype(producer)>, senders[i], data,
                                                std::ref(producerCount), std::ref(producerStop)));
      }
      std::vector<std::future<size_t>> consumerResult;
      const size_t numberConsumers = 1;
      consumerResult.reserve(numberConsumers);
      consumerResult.emplace_back(std::async(std::launch::async, GetUntil<decltype(consumer)>, consumer, data,
                                             std::ref(consumerCount), std::ref(consumerStop)));

      using namespace std::chrono_literals;
      while (consumerCount.load() < numberConsumers && producerCount.load() < numberProducers) {
         std::this_thread::sleep_for(1us);
      }
      StopWatch elapsedRun;
      while (elapsedRun.ElapsedSec() < timeToRunInSec) {
         std::this_thread::sleep_for(1us);
      }

      producerStop.store(true, std::memory_order_release);
      size_t amountProduced = 0;
      for (auto& result : producerResult) {
         amountProduced += result.get();
      }
      consumerStop.store(true, std::memory_order_release);
      size_t amountConsumed = 0;
      for (auto& result : consumerResult) {
         amountConsumed += result.get();
      }

      // amoundProduced >= amountConsumed
      // amountProduced <= amountConsumed + 100
      EXPECT_GE(amountProduced, amountConsumed) << "produced: " << amountProduced << ", consumed: " << amountConsumed << ", capacity: " << consumer.capacity();

      auto elapsedTimeNs = elapsedRun.ElapsedNs();
      auto elapsedTimeSec = elapsedTimeNs / (1000000000);
      std::cout << "Transaction/s: " << amountConsumed / elapsedTimeSec << std::endl;
      std::cout << "Average transaction: " << elapsedTimeNs / amountConsumed << " ns" << std::endl;

      std::cout << "Transaction/s per consumer: " << amountConsumed / elapsedTimeSec / numberConsumers << std::endl;
      std::cout << "Transation GByte/s: " << amountConsumed * data.size() / (1024 * 1024 * 1024) / elapsedTimeSec << std::endl;
   }

   template <typename QType, typename QTypePair>
   void RunSPMC(std::vector<QTypePair> queues, std::string data, const size_t timeToRunInSec) {
      std::atomic<size_t> producerCount{0};
      std::atomic<size_t> consumerCount{0};
      std::atomic<bool> producerStop{false};
      std::atomic<bool> consumerStop{false};

      std::vector<queue_api::Receiver<QType>> receivers;
      std::vector<queue_api::Sender<QType>> senders;

      for (auto& q : queues) {
         receivers.push_back(std::get<queue_api::index::receiver>(q));
         senders.push_back(std::get<queue_api::index::sender>(q));
      }
      const size_t numberConsumers = senders.size();
      spmc::round_robin::Sender<QType> producer(senders);
      std::vector<std::future<size_t>> producerResult;
      const size_t numberProducers = 1;
      producerResult.reserve(numberProducers);
      producerResult.emplace_back(std::async(std::launch::async, PushUntil<decltype(producer)>, producer, data,
                                             std::ref(producerCount), std::ref(producerStop)));

      std::vector<std::future<size_t>> consumerResult;
      consumerResult.reserve(receivers.size());
      for (size_t i = 0; i < receivers.size(); ++i) {
         auto consumer = receivers[i];
         consumerResult.emplace_back(std::async(std::launch::async, GetUntil<decltype(consumer)>, receivers[i], data,
                                                std::ref(consumerCount), std::ref(consumerStop)));
      }

      using namespace std::chrono_literals;
      while (consumerCount.load() < numberConsumers && producerCount.load() < numberProducers) {
         std::this_thread::sleep_for(1us);
      }
      StopWatch elapsedRun;
      while (elapsedRun.ElapsedSec() < timeToRunInSec) {
         std::this_thread::sleep_for(1us);
      }

      producerStop.store(true, std::memory_order_release);
      size_t amountProduced = 0;
      for (auto& result : producerResult) {
         amountProduced += result.get();
      }
      consumerStop.store(true, std::memory_order_release);
      size_t amountConsumed = 0;
      for (auto& result : consumerResult) {
         amountConsumed += result.get();
      }

      // amoundProduced >= amountConsumed
      // amountProduced <= amountConsumed + 100
      EXPECT_GE(amountProduced, amountConsumed) << "produced: " << amountProduced << ", consumed: " << amountConsumed << ", capacity: " << producer.capacity();

      auto elapsedTimeNs = elapsedRun.ElapsedNs();
      auto elapsedTimeSec = elapsedTimeNs / (1000000000);
      std::cout << "Transaction/s: " << amountConsumed / elapsedTimeSec << std::endl;
      std::cout << "Average transaction: " << elapsedTimeNs / amountConsumed << " ns" << std::endl;

      std::cout << "Transaction/s per consumer: " << amountConsumed / elapsedTimeSec / numberConsumers << std::endl;
      std::cout << "Transation GByte/s: " << amountConsumed * data.size() / (1024 * 1024 * 1024) / elapsedTimeSec << std::endl;
   }

}  // namespace test_performance