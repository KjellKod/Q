/* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* Originally published at: https://github.com/KjellKod/Q
*/

#include <test_performance.hpp>
#include <q/spsc.hpp>
#include <q/mpmc.hpp>
#include <thread>
#include <algorithm>

namespace {
   const size_t kAmount = 1000000;
   const size_t kSmallQueueSize = 100;
}

using namespace test_performance;

TEST(Performance, SPSC_Flexible_CircularFifo) {
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(kAmount);
   RunSPSC(queue, kAmount);
}


TEST(Performance, SPSC_Flexible_CircularFifo_Smaller) {
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(kSmallQueueSize);
   RunSPSC(queue, kAmount);
}

TEST(Performance, SPSC_Fixed_CircularFifo) {
   using namespace std;
   auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<string, kSmallQueueSize>>();
   RunSPSC(queue, kAmount);
}


TEST(Performance, SPSC_Fixed_CircularFifo_Smaller) {
   using namespace std;
   auto queue = queue_api::CreateQueue < spsc::fixed::circular_fifo < string, kSmallQueueSize>> ();
   RunSPSC(queue, kAmount);
}



TEST(Performance, MPMC_1_to_1) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kAmount);
   RunSPSC(queue, kAmount);
}


TEST(Performance, MPMC_1_to_1_Smaller) {
   using namespace std;

   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kSmallQueueSize);
   RunSPSC(queue, kAmount);
}


TEST(Performance, SPSC_Flexible_20secRun_LargeData) {
   using namespace std;
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(kSmallQueueSize);
   const size_t large = 65000;
   std::string payload(large, 'x');
   EXPECT_EQ(large, payload.size());
   const size_t numberOfProducers = 1;
   const size_t numberOfConsumers = 1;
   const size_t kTimeToRunSec = 20;
   RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
}

TEST(Performance, SPSC_Fixed_20secRun_LargeData) {
   using namespace std;
   auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<std::string, kSmallQueueSize>>();
   const size_t large = 65000;
   std::string payload(large, 'x');
   EXPECT_EQ(large, payload.size());
   const size_t numberOfProducers = 1;
   const size_t numberOfConsumers = 1;
   const size_t kTimeToRunSec = 20;
   RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
}


TEST(Performance, MPMC_1_to_4_20secRun_LargeData) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kSmallQueueSize);
   const size_t large = 65000;
   std::string payload(large, 'x');
   EXPECT_EQ(large, payload.size());
   const size_t numberOfProducers = 1;
   const size_t numberOfConsumers = 4;
   const size_t kTimeToRunSec = 20;
   RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
}

TEST(Performance, MPMC_4_to_1_20secRun_LargeData) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kSmallQueueSize);
   const size_t large = 65000;
   std::string payload(large, 'x');
   EXPECT_EQ(large, payload.size());
   const size_t numberOfProducers = 1;
   const size_t numberOfConsumers = 4;
   const size_t kTimeToRunSec = 20;
   RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
}


TEST(Performance, MPMC_4_to_4_20secRun_LargeData) {
   using namespace std;
   auto queue = queue_api::CreateQueue<mpmc::dynamic_lock_queue<string>>(kSmallQueueSize);
   const size_t large = 65000;
   std::string payload(large, 'x');
   EXPECT_EQ(large, payload.size());
   const size_t numberOfProducers = 4;
   const size_t numberOfConsumers = 4;
   const size_t kTimeToRunSec = 20;
   RunMPMC(queue, payload, numberOfProducers, numberOfConsumers, kTimeToRunSec);
}

