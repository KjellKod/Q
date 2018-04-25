#include <gtest/gtest.h>
#include "q/spsc.hpp"
#include "q/mpmc.hpp"
#include "q/q_api.hpp"
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>

namespace {
   const size_t kAmount = 1000000;
   const size_t kSmallQueueSize = 100;
   std::atomic<bool> producerStart{false};
   std::atomic<bool> consumerStart{false};
   using ResultType = std::vector<std::string>;
}

void wait() {
   using namespace std::chrono_literals;
   std::this_thread::sleep_for(2ns);
}

template <typename Sender>
ResultType Push(Sender q, size_t start, size_t stop) {
   std::vector<std::string> expected;
   expected.reserve(stop - start);
   producerStart.store(true);
   while (!consumerStart.load()) {
      wait();
   }

   for (auto i = start; i < stop; ++i) {
      std::string value = std::to_string(i);
      expected.push_back(value);
      while (false == q.push(value)) {
         std::this_thread::yield();
      }
   }
   return expected;
}

template <typename Receiver>
ResultType Get(Receiver q, size_t start, size_t stop) {
   std::vector<std::string> received;
   received.reserve(stop - start);
   consumerStart.store(true);
   while (!producerStart.load()) {
      wait();
   }

   for (auto i = start; i < stop; ++i) {
      std::string value;
      while (false == q.pop(value)) {
         std::this_thread::yield();
      }
      received.push_back(value);
   }
   return received;
}



template<typename T>
void RunSPSC(T queue) {
   producerStart.store(false);
   consumerStart.store(false);

   using namespace std;
   using namespace chrono;
   auto producer = std::get <queue_api::index::sender>(queue);
   auto consumer = std::get <queue_api::index::receiver>(queue);

   auto t1 = high_resolution_clock::now();
   size_t start = 0;
   size_t stop = kAmount;
   auto prodResult = std::async(std::launch::async, Push<decltype(producer)>,
                                producer, start, stop);
   auto consResult = std::async(std::launch::async, Get<decltype(consumer)>,
                                consumer, start, stop);

   auto expected = prodResult.get();
   auto received = consResult.get();
   auto t2 = high_resolution_clock::now();
   auto us = duration_cast<microseconds>( t2 - t1 ).count();
   std::cout << "Push - Pull #" << kAmount << " items in: " << us  << " us" << std::endl;
   std::cout << "Average: " << 1000 * ((float)us / (float) kAmount) << " ns" << std::endl;

   EXPECT_EQ(kAmount, received.size());
   EXPECT_EQ(expected, received);
}



TEST(Performance, SPSC_Flexible_CircularFifo) {
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(kAmount);
   RunSPSC(queue);
}


TEST(Performance, SPSC_Flexible_CircularFifo_Smaller) {
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(kSmallQueueSize);
   RunSPSC(queue);
}

TEST(Performance, SPSC_Fixed_CircularFifo) {
   using namespace std;
   auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<string, kSmallQueueSize>>();
   RunSPSC(queue);
}


TEST(Performance, SPSC_Fixed_CircularFifo_Smaller) {
   using namespace std;
   auto queue = queue_api::CreateQueue < spsc::fixed::circular_fifo < string, kSmallQueueSize>> ();
   RunSPSC(queue);
}



TEST(Performance, MPMC_1_to_1) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kAmount, std::chrono::milliseconds(1));
   RunSPSC(queue);
}


TEST(Performance, MPMC_1_to_1_Smaller) {
   using namespace std;

   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kSmallQueueSize,
                std::chrono::milliseconds(1));
   RunSPSC(queue);
}


template<typename T>
void RunMPSC(T queue) {
   producerStart.store(false);
   consumerStart.store(false);
   
   using namespace std;
   using namespace std::chrono;

   auto producer = std::get <queue_api::index::sender>(queue);
   auto consumer = std::get <queue_api::index::receiver>(queue);


   auto start1 = 0;
   auto stop1 = kAmount / 4 * 1;

   auto start2 = kAmount / 4 * 1;
   auto stop2 = kAmount / 4 * 2;

   auto start3 = kAmount / 4 * 2;
   auto stop3 = kAmount / 4 * 3;

   auto start4 = kAmount / 4 * 3;
   auto stop4 = kAmount / 4 * 4;

   auto start0 = 0;
   auto stop0 = kAmount;

   auto t1 = high_resolution_clock::now();

   auto p1 = std::async(std::launch::async, Push<decltype(producer)>, producer, start1, stop1);
   auto p2 = std::async(std::launch::async, Push<decltype(producer)>, producer, start2, stop2);
   auto p3 = std::async(std::launch::async, Push<decltype(producer)>, producer, start3, stop3);
   auto p4 = std::async(std::launch::async, Push<decltype(producer)>, producer, start4, stop4);
   auto c0 = std::async(std::launch::async, Get<decltype(consumer)>, consumer, start0, stop0);

   auto e1 = p1.get();
   auto e2 = p2.get();
   auto e3 = p3.get();
   auto e4 = p4.get();
   auto received =  c0.get();
   auto t2 = high_resolution_clock::now();

   auto us = duration_cast<microseconds>( t2 - t1 ).count();
   std::cout << "Push - Pull #" << kAmount << " items in: " << us  << " us" << std::endl;
   std::cout << "Average: " << 1000 * ((float)us / (float) kAmount) << " ns" << std::endl;


   std::vector<std::string> eAll;
   //eAll.reserve(kAmount);
   eAll.insert(eAll.end(), e1.begin(), e1.end());
   e1.clear();
   eAll.insert(eAll.end(), e2.begin(), e2.end());
   e2.clear();
   eAll.insert(eAll.end(), e3.begin(), e3.end());
   e3.clear();
   eAll.insert(eAll.end(), e4.begin(), e4.end());
   e4.clear();

   std::sort(eAll.begin(), eAll.end());
   std::sort(received.begin(), received.end());
   EXPECT_EQ(kAmount, received.size());
   EXPECT_EQ(eAll, received);
}


TEST(Performance, MPMC_4_to_1) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kAmount,
                std::chrono::milliseconds(1));
   RunMPSC(queue);
}

TEST(Performance, MPMC_4_to_1_Smaller) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kSmallQueueSize,
                std::chrono::milliseconds(1));
   RunMPSC(queue);
}
