/* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
* Kjell Hedstrom, 2018
*/

#include <gtest/gtest.h>
#include <string>
#include "q/mpsc_receiver_round_robin.hpp"
#include "q/q_api.hpp"
#include "q/spmc_sender_round_robin.hpp"
#include "q/spsc_flexible_circular_fifo.hpp"

TEST(MPSC_SPMC, CreateOneQueue_MPSC) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto queue = queue_api::CreateQueue<qtype>(10);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto temporary = std::get<queue_api::index::receiver>(queue);

   // convert the setup to a MPSC setup
   mpsc::round_robin::Receiver<qtype> consumer({temporary});

   std::string e;
   EXPECT_FALSE(consumer.pop(e));
   EXPECT_TRUE(consumer.empty());
   EXPECT_FALSE(consumer.full());
   EXPECT_EQ(10, consumer.capacity());
   EXPECT_EQ(10, consumer.capacity_free());
   EXPECT_EQ(0, consumer.size());
   EXPECT_TRUE(consumer.lock_free());
}

TEST(MPSC_SPMC, CreateManyQueues) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   constexpr auto senderID = queue_api::index::sender;
   constexpr auto receiverID = queue_api::index::receiver;

   constexpr size_t kSize = 10;
   constexpr size_t kSizeTotal = kSize * kSize;
   std::vector<queue_api::Sender<qtype>> senders;
   std::vector<queue_api::Receiver<qtype>> receivers;
   for (size_t i = 0; i < 10; ++i) {
      auto queue = queue_api::CreateQueue<qtype>(10);
      senders.push_back(std::get<senderID>(queue));
      receivers.push_back(std::get<receiverID>(queue));
   }

   mpsc::round_robin::Receiver<qtype> consumer({receivers});
   EXPECT_EQ(kSizeTotal, consumer.capacity());
   EXPECT_EQ(kSizeTotal, consumer.capacity_free());
}

TEST(MPSC_SPMC, RoundRobinOfOne) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto queue = queue_api::CreateQueue<qtype>(2);
   auto temporary = std::get<queue_api::index::receiver>(queue);
   // convert the setup to a MPSC setup
   mpsc::round_robin::Receiver<qtype> consumer({temporary});

   size_t current = 0;
   current = consumer.increment(current);
   EXPECT_EQ(0, current);
   current = consumer.increment(current);
   EXPECT_EQ(0, current);
}

TEST(MPSC_SPMC, RoundRobinOfMany) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto q1 = queue_api::CreateQueue<qtype>(2);
   auto q2 = queue_api::CreateQueue<qtype>(2);
   auto r1 = std::get<queue_api::index::receiver>(q1);
   auto r2 = std::get<queue_api::index::receiver>(q2);

   // convert the setup to a MPSC setup
   //
   mpsc::round_robin::Receiver<qtype> consumer({r1, r2});

   size_t current = 0;
   current = consumer.increment(current);
   EXPECT_EQ(1, current);
   current = consumer.increment(current);
   EXPECT_EQ(0, current);
   current = consumer.increment(current);
   EXPECT_EQ(1, current);
}

TEST(MPSC_SPMC, full) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto q1 = queue_api::CreateQueue<qtype>(1);
   auto q2 = queue_api::CreateQueue<qtype>(1);
   auto r1 = std::get<queue_api::index::receiver>(q1);
   auto s1 = std::get<queue_api::index::sender>(q1);
   auto r2 = std::get<queue_api::index::receiver>(q2);
   auto s2 = std::get<queue_api::index::sender>(q2);

   // convert the setup to a MPSC setup
   //
   mpsc::round_robin::Receiver<qtype> consumer({r1, r2});
   std::string arg;
   s1.push(arg);
   EXPECT_TRUE(r1.full());
   EXPECT_FALSE(r2.full());
   EXPECT_FALSE(consumer.full());

   s2.push(arg);
   EXPECT_TRUE(r1.full());
   EXPECT_TRUE(r2.full());
   EXPECT_TRUE(consumer.full());
}

TEST(MPSC_SPMC, size) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto q1 = queue_api::CreateQueue<qtype>(1);
   auto q2 = queue_api::CreateQueue<qtype>(1);
   auto r1 = std::get<queue_api::index::receiver>(q1);
   auto s1 = std::get<queue_api::index::sender>(q1);
   auto r2 = std::get<queue_api::index::receiver>(q2);
   auto s2 = std::get<queue_api::index::sender>(q2);

   // convert the setup to a MPSC setup
   //
   mpsc::round_robin::Receiver<qtype> consumer({r1, r2});
   std::string arg;
   EXPECT_EQ(2, consumer.capacity());
   EXPECT_EQ(2, consumer.capacity_free());
   s1.push(arg);
   EXPECT_EQ(1, consumer.size());
   EXPECT_EQ(1, consumer.capacity_free());

   s2.push(arg);
   EXPECT_EQ(2, consumer.size());
   EXPECT_EQ(0, consumer.capacity_free());
}

TEST(MPSC_SPMC, pop) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto q1 = queue_api::CreateQueue<qtype>(1);
   auto q2 = queue_api::CreateQueue<qtype>(1);
   auto r1 = std::get<queue_api::index::receiver>(q1);
   auto s1 = std::get<queue_api::index::sender>(q1);
   auto r2 = std::get<queue_api::index::receiver>(q2);
   auto s2 = std::get<queue_api::index::sender>(q2);

   // convert the setup to a MPSC setup
   //
   mpsc::round_robin::Receiver<qtype> consumer({r1, r2});
   std::string arg = "s0";
   s1.push(arg);

   std::string recv;
   EXPECT_TRUE(consumer.pop(recv));
   EXPECT_EQ("s0", recv);

   arg = "s1";
   s1.push(arg);
   arg = "s2";
   s2.push(arg);

   EXPECT_TRUE(consumer.pop(recv));
   EXPECT_EQ("s2", recv);
   EXPECT_TRUE(consumer.pop(recv));
   EXPECT_EQ("s1", recv);

   recv = "";
   EXPECT_FALSE(consumer.pop(recv));
   EXPECT_EQ("", recv);
}

TEST(MPSC_SPMC, usage) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto q1 = queue_api::CreateQueue<qtype>(1);
   auto q2 = queue_api::CreateQueue<qtype>(1);
   auto r1 = std::get<queue_api::index::receiver>(q1);
   auto s1 = std::get<queue_api::index::sender>(q1);
   auto r2 = std::get<queue_api::index::receiver>(q2);
   auto s2 = std::get<queue_api::index::sender>(q2);

   // convert the setup to a MPSC setup
   //
   mpsc::round_robin::Receiver<qtype> consumer({r1, r2});
   std::string arg;
   EXPECT_EQ(0, consumer.usage());
   s1.push(arg);
   EXPECT_EQ(50, consumer.usage());

   s2.push(arg);
   EXPECT_EQ(100, consumer.usage());
}

TEST(MPSC_SPMC, CreateOneQueue_SPMC) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto queue = queue_api::CreateQueue<qtype>(10);
   auto temporary = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);

   // convert the setup to a MPSC setup
   spmc::round_robin::Sender<qtype> producer({temporary});

   std::string e;
   EXPECT_TRUE(producer.empty());
   EXPECT_FALSE(producer.full());
   EXPECT_EQ(10, producer.capacity());
   EXPECT_EQ(10, producer.capacity_free());
   EXPECT_EQ(0, producer.size());
   EXPECT_TRUE(producer.lock_free());
   EXPECT_TRUE(producer.push(e));
}

TEST(MPSC_SPMC, CreateManyQueues_SPMC) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   constexpr auto senderID = queue_api::index::sender;
   constexpr auto receiverID = queue_api::index::receiver;

   constexpr size_t kSize = 10;
   constexpr size_t kSizeTotal = kSize * kSize;
   std::vector<queue_api::Sender<qtype>> senders;
   std::vector<queue_api::Receiver<qtype>> receivers;
   for (size_t i = 0; i < 10; ++i) {
      auto queue = queue_api::CreateQueue<qtype>(10);
      senders.push_back(std::get<senderID>(queue));
      receivers.push_back(std::get<receiverID>(queue));
   }

   spmc::round_robin::Sender<qtype> consumer({senders});
   EXPECT_EQ(kSizeTotal, consumer.capacity());
   EXPECT_EQ(kSizeTotal, consumer.capacity_free());
}

TEST(MPSC_SPMC, push_SPMC) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto q1 = queue_api::CreateQueue<qtype>(1);
   auto q2 = queue_api::CreateQueue<qtype>(1);
   auto r1 = std::get<queue_api::index::receiver>(q1);
   auto s1 = std::get<queue_api::index::sender>(q1);
   auto r2 = std::get<queue_api::index::receiver>(q2);
   auto s2 = std::get<queue_api::index::sender>(q2);

   // convert the setup to a MPSC setup
   //
   spmc::round_robin::Sender<qtype> producer({s1, s2});
   std::string arg = "s0";
   producer.push(arg);

   std::string recv;
   EXPECT_FALSE(r2.pop(recv));
   EXPECT_TRUE(r1.pop(recv));
   EXPECT_EQ("s0", recv);

   arg = "s1";
   producer.push(arg);
   arg = "s2";
   producer.push(arg);

   EXPECT_TRUE(r2.pop(recv));
   EXPECT_EQ("s1", recv);
   EXPECT_TRUE(r1.pop(recv));
   EXPECT_EQ("s2", recv);

   EXPECT_TRUE(producer.push(arg));
   EXPECT_TRUE(producer.push(arg));
   EXPECT_FALSE(producer.push(arg));
}
