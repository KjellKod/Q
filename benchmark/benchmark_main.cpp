#include <iostream>
#include "benchmark_functions.hpp"
#include "benchmark_runs.hpp"
#include "q/q_api.hpp"
#include "q/spsc_flexible_circular_fifo.hpp"

namespace {
   const size_t kGoodSizedQueueSize = (2 << 16);  // 65536
   const size_t kNumberOfItems = 1000000;

}  // namespace

void benchmark_spsc() {
   std::vector<benchmark::result_t> results;
   double total_duration_us = 0.0;
   for (int i = 0; i < 20; ++i) {
      auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<unsigned int>>(kGoodSizedQueueSize);
      auto result = benchmark::runSPSC(queue, kNumberOfItems);
      results.push_back(result);
      total_duration_us += result.elapsed_time_in_us;
   }

   double total_duration_s = total_duration_us / 1e6;                   // convert microseconds to seconds
   double calls_per_second = kNumberOfItems / (total_duration_s / 20);  // average calls per second

   double total_duration_ns = total_duration_us * 1e3;                           // convert microseconds to nanoseconds
   double average_call_duration_ns = total_duration_ns / (kNumberOfItems * 20);  // average call duration in nanoseconds

   std::cout << "Calls per second: " << calls_per_second << std::endl;
   std::cout << "Average call duration: " << average_call_duration_ns << " ns" << std::endl;
}

int main() {
   benchmark_spsc();
   std::cout << "Hello World\n";
   return 0;
}