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
   const int kRuns = 33;
   std::vector<benchmark::result_t> results;
   double total_duration_ns = 0.0;
   for (int i = 0; i < kRuns; ++i) {
      benchmark::stopwatch watch;
      auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<unsigned int>>(kGoodSizedQueueSize);
      auto result = benchmark::runSPSC(queue, kNumberOfItems);
      results.push_back(result);
      Q_CHECK(watch.elapsed_ns() > result.elapsed_time_in_ns);
      total_duration_ns += result.elapsed_time_in_ns;
      std::cout << "Run " << i << " took " << watch.elapsed_ms() << " ms" << std::endl;
   }

   double total_duration_s = total_duration_ns / 1e9;  // convert ns to seconds
   std::cout << kRuns << " runs number of seconds " << total_duration_s << "\n";
   double total_duration_us = total_duration_ns / 1e3;                             // convert ns to microseconds
   double calls_per_second = (kRuns * kNumberOfItems) / (total_duration_s);        // average calls per second
   double average_ops_duration_us = total_duration_us / (kNumberOfItems * kRuns);  // average call duration in nanoseconds

   std::cout << "Calls per second: " << calls_per_second << std::endl;
   std::cout << "Average call duration: " << average_ops_duration_us << " ns" << std::endl;
}

int main() {
   benchmark_spsc();
   std::cout << "Hello World\n";
   return 0;
}