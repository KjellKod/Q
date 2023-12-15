#include <iostream>
// #include "benchmark_functions.hpp"
// #include "benchmark_runs.hpp"
// #include "q/q_api.hpp"
// #include "q/spsc_flexible_circular_fifo.hpp"

// namespace {
//    const size_t kGoodSizedQueueSize = (2 << 16);  // 65536
//    const size_t kNumberOfItems = 1000000;

// }  // namespace

void benchmark_spsc() {
   //    std::vector<benchmark::result_t> results;
   //    for (int i = 0; i < 20; ++i) {
   //       auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<unsigned int>>(kGoodSizedQueueSize);
   //       auto result = benchmark::runSPSC(queue, kNumberOfItems);
   //       results.push_back(result);
   //    }
}

int main() {
   benchmark_spsc();
   std::cout << "Hello World\n";
   return 0;
}