// /* Not any company's property but Public-Domain
// * Do with source-code as you will. No requirement to keep this
// * header if need to use it/change it/ or do whatever with it
// *
// * Note that there is No guarantee that this code will work
// * and I take no responsibility for this code and any problems you
// * might get if using it.
// *
// * Originally published at: https://github.com/KjellKod/Q
// */

// //#include "test_performance.hpp"
// #include <gtest/gtest.h>
// #include <algorithm>
// #include <future>
// #include <q/mpmc.hpp>
// #include <q/spsc.hpp>
// #include <thread>

// namespace {
//    const size_t kAmount = 1000000;
//    const size_t kSmallQueueSize = 100;
//    const size_t kGoodSizedQueueSize = (2 << 16);  // 65536

// }  // namespace

// using namespace test_performance;

// TEST(Performance, SPSC_circular_fifo_CircularFifo) {
//    auto queue = queue_api::CreateQueue<spsc::circular_fifo<std::string>>(kGoodSizedQueueSize);
//    auto result = RunSPS2C(queue, kAmount);
//    EXPECT_EQ(result.size(), 3);
//    auto sent = result[0];
//    auto received = result[1];
//    auto runner = result[2];
//    EXPECT_EQ(sent.sum, received.sum);
//    EXPECT_NE(runner.sum, received.sum);
//    std::cout << "messages / second " << kAmount/(runner.nanotime * 1000000000)
// }

// TEST(Performance, SPSC_circular_fifo_CircularFifo) {
//    auto queue = queue_api::CreateQueue<spsc::circular_fifo<std::string>>(kSmallQueueSize);
//    RunSPSC(queue, kAmount);
// }

// TEST(Performance, SPSC_circular_fifo_CircularFifo_Smaller) {
//    auto queue = queue_api::CreateQueue<spsc::circular_fifo<std::string>>(kSmallQueueSize);
//    RunSPSC(queue, kAmount);
// }

// TEST(Performance, SPSC_Fixed_CircularFifo) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<string, kSmallQueueSize>>();
//    RunSPSC(queue, kAmount);
// }

// TEST(Performance, SPSC_Fixed_CircularFifo_Smaller) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<string, kSmallQueueSize>>();
//    RunSPSC(queue, kAmount);
// }

// TEST(Performance, MPMC_1_to_1) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kAmount);
//    RunSPSC(queue, kAmount);
// }

// TEST(Performance, MPMC_1_to_1_Smaller) {
//    using namespace std;

//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kSmallQueueSize);
//    RunSPSC(queue, kAmount);
// }

// TEST(Performance, DISABLED_SPSC_circular_fifo_20secRun_LargeData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<spsc::circular_fifo<std::string>>(kSmallQueueSize);
//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t numberOfProducers = 1;
//    const size_t numberOfConsumers = 1;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_SPSC_Fixed_20secRun_LargeData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<std::string, kSmallQueueSize>>();
//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t numberOfProducers = 1;
//    const size_t numberOfConsumers = 1;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_MPMC_1_to_4_20secRun_LargeData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kSmallQueueSize);
//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t numberOfProducers = 1;
//    const size_t numberOfConsumers = 4;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_MPMC_1_to_4_20secRun_SmallData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kSmallQueueSize);
//    const size_t small = 10;
//    std::string payload(small, 'x');
//    EXPECT_EQ(small, payload.size());
//    const size_t numberOfProducers = 1;
//    const size_t numberOfConsumers = 4;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_lock_free__SPMC_1_to_4_20secRun_LargeData) {
//    using namespace std;
//    using element = std::string;
//    using qtype = spsc::circular_fifo<element>;
//    using qtype_pair = std::tuple<queue_api::Sender<qtype>, queue_api::Receiver<qtype>>;
//    std::vector<qtype_pair> queues;
//    for (size_t i = 0; i < 4; ++i) {
//       queues.push_back(queue_api::CreateQueue<qtype>(kSmallQueueSize));
//    }

//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t kTimeToRunSec = 20;
//    RunSPMC<qtype, qtype_pair>(queues, payload, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_lock_free__SPMC_1_to_4_20secRun_SmallData) {
//    using namespace std;
//    using element = std::string;
//    using qtype = spsc::circular_fifo<element>;
//    using qtype_pair = std::tuple<queue_api::Sender<qtype>, queue_api::Receiver<qtype>>;
//    std::vector<qtype_pair> queues;
//    for (size_t i = 0; i < 4; ++i) {
//       queues.push_back(queue_api::CreateQueue<qtype>(kSmallQueueSize));
//    }

//    const size_t small = 10;
//    std::string payload(small, 'x');
//    EXPECT_EQ(small, payload.size());
//    const size_t kTimeToRunSec = 20;
//    RunSPMC<qtype, qtype_pair>(queues, payload, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_MPMC_4_to_1_20secRun_LargeData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kSmallQueueSize);
//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t numberOfProducers = 4;
//    const size_t numberOfConsumers = 1;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_MPMC_4_to_1_20secRun_SmallData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kSmallQueueSize);
//    const size_t small = 10;
//    std::string payload(small, 'x');
//    EXPECT_EQ(small, payload.size());
//    const size_t numberOfProducers = 4;
//    const size_t numberOfConsumers = 1;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_lock_free__MPSC_4_to_1_20secRun_LargeData) {
//    using namespace std;
//    using element = std::string;
//    using qtype = spsc::circular_fifo<element>;
//    using qtype_pair = std::tuple<queue_api::Sender<qtype>, queue_api::Receiver<qtype>>;
//    std::vector<qtype_pair> queues;
//    for (size_t i = 0; i < 4; ++i) {
//       queues.push_back(queue_api::CreateQueue<qtype>(kSmallQueueSize));
//    }

//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t kTimeToRunSec = 20;
//    RunMPSC<qtype, qtype_pair>(queues, payload, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_lock_free__MPSC_4_to_1_20secRun_SmallData) {
//    using namespace std;
//    using element = std::string;
//    using qtype = spsc::circular_fifo<element>;
//    using qtype_pair = std::tuple<queue_api::Sender<qtype>, queue_api::Receiver<qtype>>;
//    std::vector<qtype_pair> queues;
//    for (size_t i = 0; i < 4; ++i) {
//       queues.push_back(queue_api::CreateQueue<qtype>(kSmallQueueSize));
//    }

//    const size_t small = 10;
//    std::string payload(small, 'x');
//    EXPECT_EQ(small, payload.size());
//    const size_t kTimeToRunSec = 20;
//    RunMPSC<qtype, qtype_pair>(queues, payload, kTimeToRunSec);
// }

// TEST(Performance, DISABLED_MPMC_4_to_4_20secRun_LargeData) {
//    using namespace std;
//    auto queue = queue_api::CreateQueue<mpmc::lock_queue<string>>(kSmallQueueSize);
//    const size_t large = 65000;
//    std::string payload(large, 'x');
//    EXPECT_EQ(large, payload.size());
//    const size_t numberOfProducers = 4;
//    const size_t numberOfConsumers = 4;
//    const size_t kTimeToRunSec = 20;
//    RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
// }
