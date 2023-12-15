#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include "benchmark_functions.hpp"
#include "q/q_api.hpp"

namespace benchmark {
   template <typename T>
   benchmark::result_t runSPSC(T queue, size_t howMany) {
      std::atomic<bool> producerStart{false};
      std::atomic<bool> consumerStart{false};

      using namespace std;
      using namespace chrono;
      auto producer = std::get<queue_api::index::sender>(queue);
      auto consumer = std::get<queue_api::index::receiver>(queue);

      benchmark::stopwatch watch;
      size_t stop = howMany;
      auto prodResult = std::async(std::launch::async, benchmark::Push<decltype(producer)>,
                                   producer, stop, std::ref(producerStart), std::ref(consumerStart));
      auto consResult = std::async(std::launch::async, benchmark::Get<decltype(consumer)>,
                                   consumer, stop, std::ref(producerStart), std::ref(consumerStart));

      auto sent = prodResult.get();
      auto received = consResult.get();
      Q_CHECK(producer.empty());
      Q_CHECK(consumer.empty());
      Q_CHECK_EQ(sent.total_sum, received.total_sum);
      Q_CHECK(watch.elapsed_ns() >= sent.elapsed_time_in_ns);
      Q_CHECK(watch.elapsed_ns() >= received.elapsed_time_in_ns);
      return {received.total_sum,std::max(sent.elapsed_time_in_ns, received.elapsed_time_in_ns)};
   }
}  // namespace benchmark