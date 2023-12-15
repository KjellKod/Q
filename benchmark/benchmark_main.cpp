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
      auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<unsigned int>>(kGoodSizedQueueSize);
      auto result = benchmark::runSPSC(queue, kNumberOfItems);
      results.push_back(result);
      total_duration_ns += result.elapsed_time_in_ns;
   }

   double total_duration_s = total_duration_ns / 1e9;  // convert ns to seconds
   std::cout << kRuns << " runs x " << kNumberOfItems << " took " << total_duration_s << " seconds \n";
   double calls_per_second = (kRuns * kNumberOfItems) / (total_duration_s);        // average calls per second
   double average_ops_duration_ns = total_duration_ns / (kNumberOfItems * kRuns);  // average call duration in nanoseconds

   std::cout << "Calls per second: " << calls_per_second << std::endl;
   std::cout << "Average push/pop duration: " << average_ops_duration_ns << " ns" << std::endl;
}

int main() {
   benchmark_spsc();
   return 0;
}