#include <iomanip>
#include <iostream>
#include "benchmark_functions.hpp"
#include "benchmark_runs.hpp"
#include "q/q_api.hpp"
#include "q/spsc_flexible_circular_fifo.hpp"

namespace {
   const size_t kGoodSizedQueueSize = (2 << 16);  // 65536
   const size_t kNumberOfItems = 1000000;

   struct benchmark_result {
      int runs;
      int num_producer_threads;
      int num_consumer_threads;
      size_t messages_per_iteration;
      double mean_msgs_per_second;
      double min_msgs_per_second;
      double max_msgs_per_second;
      std::string comment;
   };
}  // namespace

void print_result(const benchmark_result& result) {
   std::cout << std::setw(2) << result.runs << ", "
             << std::setw(5) << result.num_producer_threads << ", "
             << std::setw(6) << result.num_consumer_threads << ", "
             << std::setw(16) << result.mean_msgs_per_second << ", "
             << std::setw(14) << result.min_msgs_per_second << ", "
             << std::setw(14) << result.max_msgs_per_second << ", "
             << std::setw(17) << result.comment << std::endl;
}

benchmark_result benchmark_spsc() {
   const int kRuns = 33;
   std::vector<benchmark::result_t> results;
   double total_duration_ns = 0.0;
   double min_msgs_per_second = std::numeric_limits<double>::max();
   double max_msgs_per_second = std::numeric_limits<double>::min();
   double total_msgs_per_second = 0.0;

   for (int i = 0; i < kRuns; ++i) {
      auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<unsigned int>>(kGoodSizedQueueSize);
      auto result = benchmark::runSPSC(queue, kNumberOfItems);
      results.push_back(result);
      total_duration_ns += result.elapsed_time_in_ns;

      double duration_s = result.elapsed_time_in_ns / 1e9;   // convert ns to seconds
      double msgs_per_second = kNumberOfItems / duration_s;  // messages per second for this run
      total_msgs_per_second += msgs_per_second;
      min_msgs_per_second = std::min(min_msgs_per_second, msgs_per_second);
      max_msgs_per_second = std::max(max_msgs_per_second, msgs_per_second);
   }

   double mean_msgs_per_second = total_msgs_per_second / kRuns;  // mean messages per second

   benchmark_result result;
   result.runs = kRuns;
   result.num_producer_threads = 1;
   result.num_consumer_threads = 1;
   result.messages_per_iteration = kNumberOfItems;
   result.mean_msgs_per_second = mean_msgs_per_second;
   result.min_msgs_per_second = min_msgs_per_second;
   result.max_msgs_per_second = max_msgs_per_second;
   result.comment = "SPSC benchmark";

   return result;
}

int main() {
   // Print the headers
   std::cout << "#runs,\t#p,\t#c,\t#msgs/s,\t#min_msgs/s,\t#max_msgs/s,\tcomment" << std::endl;
   auto spsc_result = benchmark_spsc();
   print_result(spsc_result);

   return 0;
}