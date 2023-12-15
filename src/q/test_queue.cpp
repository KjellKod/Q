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
#include <gtest/gtest.h>
#include <q/mpmc.hpp>
#include <q/q_api.hpp>
#include <q/spsc.hpp>
#include <string>
#include "stopwatch.hpp"

using namespace std;
using Type = string;
using FlexibleQ = spsc::flexible::circular_fifo<Type>;
using FixedQ = spsc::fixed::circular_fifo<Type, 100>;
using FixedSmallQ = spsc::fixed::circular_fifo<Type, 2>;
using LockedQ = mpmc::lock_queue<Type>;

template <typename Prod, typename Cons>
void ProdConsInitialization(Prod& prod, Cons& cons) {
   EXPECT_TRUE(prod.empty());
   EXPECT_TRUE(cons.empty());

   EXPECT_FALSE(prod.full());
   EXPECT_FALSE(cons.full());

   EXPECT_TRUE(prod.lock_free());
   EXPECT_TRUE(cons.lock_free());

   EXPECT_EQ(10, prod.capacity());
   EXPECT_EQ(10, cons.capacity());

   EXPECT_EQ(10, prod.capacity_free());
   EXPECT_EQ(10, cons.capacity_free());

   EXPECT_EQ(0, prod.size());
   EXPECT_EQ(0, cons.size());
}

TEST(Queue, ProdConsInitialization) {
   auto queue = queue_api::CreateQueue<FlexibleQ>(10);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
}

TEST(Queue, ProdConsInitializationCopy) {
   using namespace queue_api;
   using QueuePair = std::pair<Sender<FlexibleQ>, Receiver<FlexibleQ>>;
   QueuePair queue = CreateQueue<FlexibleQ>(10);
   Sender<FlexibleQ> sender1 = std::get<index::sender>(queue);
   Sender<FlexibleQ> sender2(std::get<index::sender>(queue));
   Receiver<FlexibleQ> receiver1 = std::get<index::receiver>(queue);
   Receiver<FlexibleQ> receiver2(std::get<index::receiver>(queue));
}

struct HasWaitAndPop {
   std::chrono::milliseconds value;
   std::string element;
   HasWaitAndPop() :
       value(0) {}
   bool wait_and_pop(std::string& x, std::chrono::milliseconds wait_ms) {
      value = wait_ms;
      element = x;
      return false;
   }

   bool pop(std::string& x) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10s);
      return true;
   }
};

struct HasPop {
   size_t value;
   std::string element;
   HasPop() :
       value(0) {}
   bool pop(std::string& x) {
      element = x;
      return false;  // to ensure that the wait is triggered
   }
};

struct HasWaitAndPush {
   std::chrono::milliseconds value;
   std::string element;
   HasWaitAndPush() :
       value(0) {}
   bool wait_and_push(std::string& x, std::chrono::milliseconds wait_ms) {
      value = wait_ms;
      element = x;
      return false;
   }

   bool push(std::string& x) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10s);
      return true;
   }
};

struct HasPush {
   size_t value;
   std::string element;
   HasPush() :
       value(0) {}
   bool push(std::string& x) {
      element = x;
      return false;  // to ensure that the wait is triggered
   }
};

TEST(Queue, BaseAPI_Flexible) {
   auto queue = queue_api::CreateQueue<FlexibleQ>(10);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   EXPECT_TRUE(producer.empty());
   EXPECT_FALSE(producer.full());
   EXPECT_EQ(10, producer.capacity());
   EXPECT_EQ(10, producer.capacity_free());
   EXPECT_EQ(0, producer.size());
   EXPECT_TRUE(producer.lock_free());
   EXPECT_EQ(0, producer.usage());
}

TEST(Queue, BaseAPI_Fixed) {
   auto queue = queue_api::CreateQueue<FixedQ>();
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   EXPECT_TRUE(producer.empty());
   EXPECT_FALSE(producer.full());
   EXPECT_EQ(100, producer.capacity());
   EXPECT_EQ(100, producer.capacity_free());
   EXPECT_EQ(0, producer.size());
   EXPECT_TRUE(producer.lock_free());
   EXPECT_EQ(0, producer.usage());
}

TEST(Queue, BaseAPI_DynamicLocked) {
   auto queue = queue_api::CreateQueue<LockedQ>(10);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   EXPECT_TRUE(producer.empty());
   EXPECT_FALSE(producer.full());
   EXPECT_EQ(10, producer.capacity());
   EXPECT_EQ(10, producer.capacity_free());
   EXPECT_EQ(0, producer.size());
   EXPECT_FALSE(producer.lock_free());  // NOT lock -free
   EXPECT_EQ(0, producer.usage());
}

TEST(Queue, SFINAE_HasWaitAndPop) {
   auto queue = queue_api::CreateQueue<HasWaitAndPop>();
   auto consumer = std::get<queue_api::index::receiver>(queue);

   benchmark::stopwatch watch;
   std::string msg;
   std::chrono::milliseconds wait(2 * 1000);
   auto result = consumer.wait_and_pop(msg, wait);  // wait_and_pop but the bogus mock function ignores the 'wait'. We only verify SFINAE
   EXPECT_FALSE(result);
   EXPECT_TRUE(watch.elapsed_sec() <= 2);
}

TEST(Queue, SFINAE_HasPop) {
   auto queue = queue_api::CreateQueue<HasPop>();
   auto consumer = std::get<queue_api::index::receiver>(queue);

   benchmark::stopwatch watch;
   std::string msg;
   std::chrono::milliseconds wait(2 * 1000);
   auto result = consumer.wait_and_pop(msg, wait);  // wrapper implements wait. This is the actual function. we have the desired max wait.
   EXPECT_FALSE(result);
   EXPECT_TRUE(watch.elapsed_sec() >= 2);
}

TEST(Queue, SFINAE_HasWaitAndPushp) {
   auto queue = queue_api::CreateQueue<HasWaitAndPush>();
   auto producer = std::get<queue_api::index::sender>(queue);

   benchmark::stopwatch watch;
   std::string msg;
   std::chrono::milliseconds wait(2 * 1000);
   auto result = producer.wait_and_push(msg, wait);  // wait_and_pop but the bogus mock function ignores the 'wait'. We only verify SFINAE
   EXPECT_FALSE(result);
   EXPECT_TRUE(watch.elapsed_sec() <= 2);
}

TEST(Queue, SFINAE_HasPush) {
   auto queue = queue_api::CreateQueue<HasPush>();
   auto consumer = std::get<queue_api::index::sender>(queue);

   benchmark::stopwatch watch;
   std::string msg;
   std::chrono::milliseconds wait(2 * 1000);
   auto result = consumer.wait_and_push(msg, wait);  // wrapper implements wait. This is the actual function. we have the desired max wait.
   EXPECT_FALSE(result);
   EXPECT_TRUE(watch.elapsed_sec() >= 2);
}

template <typename Prod>
void QAddOne(Prod& prod) {
   std::string arg = "test";
   EXPECT_TRUE(prod.push(arg));
   EXPECT_FALSE(prod.full());
   EXPECT_EQ(100, prod.capacity());
   EXPECT_EQ(99, prod.capacity_free());
   EXPECT_EQ(1, prod.size());
   EXPECT_EQ(1, prod.tail());
}

TEST(Queue, CircularQueue_AddOne) {
   FlexibleQ dQ{100};
   QAddOne(dQ);

   FixedQ fQ{};
   QAddOne(fQ);
}

template <typename Prod, typename Cons>
void AddTillFullRemoveTillEmpty(Prod& prod, Cons& cons) {
   size_t size = 0;
   size_t free = prod._qref.capacity();
   size_t loopSize = 2;

   EXPECT_EQ(0, prod.usage());
   for (size_t i = 0; i < loopSize; ++i) {
      while (!prod.full()) {
         std::string value = to_string(i);
         EXPECT_TRUE(prod.push(value));
         ++size;
         --free;
         EXPECT_EQ((100 * prod._qref.size() / prod._qref.capacity()), prod._qref.usage());
         EXPECT_EQ(size, prod.size());
         EXPECT_EQ(free, prod.capacity_free());
      }
      EXPECT_TRUE(prod.full());
      EXPECT_TRUE(cons.full());
      EXPECT_EQ(100, prod.usage());
      EXPECT_EQ(prod.size(), prod.capacity()) << "i: " << i;
      EXPECT_EQ(cons.size(), cons.capacity());

      string t;
      while (!cons.empty()) {
         cons.pop(t);
         --size;
         ++free;
         EXPECT_EQ((100 * prod._qref.size() / prod._qref.capacity()), prod._qref.usage());
         EXPECT_EQ(size, cons.size());
         EXPECT_EQ(free, cons.capacity_free());
      }
   }
}

TEST(Queue, FlexibleQueue_AddTillFullRemoveTillEmpty) {
   auto queue = queue_api::CreateQueue<FlexibleQ>(100);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   AddTillFullRemoveTillEmpty(producer, consumer);
}

TEST(Queue, FixedQueue_AddTillFullRemoveTillEmpty) {
   auto queue = queue_api::CreateQueue<FixedQ>();
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   AddTillFullRemoveTillEmpty(producer, consumer);
}

TEST(Queue, LockedQ_AddTillFullRemoveTillEmpty) {
   auto queue = queue_api::CreateQueue<LockedQ>(100);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   AddTillFullRemoveTillEmpty(producer, consumer);
}

template <typename Prod, typename Cons>
void MoveArgument(Prod& prod, Cons& cons) {
   std::string arg = "hello";
   EXPECT_TRUE(prod.push(arg));
   EXPECT_TRUE(arg.empty());

   arg = "world";
   EXPECT_TRUE(prod.push(arg));
   EXPECT_TRUE(arg.empty());

   arg = "!";
   EXPECT_FALSE(prod.push(arg));
   EXPECT_FALSE(arg.empty());
   EXPECT_EQ("!", arg);

   EXPECT_TRUE(cons.pop(arg));
   EXPECT_EQ("hello", arg);
}

TEST(Queue, FlexibleQ_MoveArgument) {
   auto queue = queue_api::CreateQueue<FlexibleQ>(2);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   MoveArgument(producer, consumer);
}

TEST(Queue, FixedSmallQ_MoveArgument) {
   auto queue = queue_api::CreateQueue<FixedSmallQ>();
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   MoveArgument(producer, consumer);
}

TEST(Queue, LockedQ_MoveArgument) {
   auto queue = queue_api::CreateQueue<LockedQ>(2);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   MoveArgument(producer, consumer);
}

template <typename Prod, typename Cons>
void MoveUniquePtrArgument(Prod& prod, Cons& cons) {
   auto arg = make_unique<string>("hello");
   EXPECT_TRUE(prod.push(arg));
   ASSERT_TRUE(nullptr == arg);

   arg = make_unique<string>("world");
   EXPECT_TRUE(prod.push(arg));
   ASSERT_TRUE(nullptr == arg);

   arg = make_unique<string>("!");
   EXPECT_FALSE(prod.push(arg));
   EXPECT_FALSE(arg->empty());
   ASSERT_FALSE(nullptr == arg);
   EXPECT_EQ("!", *arg.get());

   EXPECT_TRUE(cons.pop(arg));
   EXPECT_EQ("hello", *arg.get());
}

namespace {
   using Unique = unique_ptr<string>;
}

TEST(Queue, FlexibleQ_MoveUnique) {
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<Unique>>(2);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   MoveUniquePtrArgument(producer, consumer);
}

TEST(Queue, FixedQ_MoveUnique) {
   auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<Unique, 2>>();
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   MoveUniquePtrArgument(producer, consumer);
}

TEST(Queue, LockedQ_MoveUnique) {
   auto queue = queue_api::CreateQueue<mpmc::lock_queue<Unique>>(2);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   MoveUniquePtrArgument(producer, consumer);
}

// Raw pointers are not transformed to nullptr
// since move semantics won't affect them
template <typename Prod, typename Cons>
void NoMovePtrArgument(Prod& prod, Cons& cons) {
   auto arg1 = make_unique<string>("hello");
   auto arg1ptr = arg1.get();
   EXPECT_TRUE(prod.push(arg1ptr));
   ASSERT_FALSE(nullptr == arg1ptr);

   auto arg2 = make_unique<string>("world");
   auto arg2ptr = arg2.get();
   EXPECT_TRUE(prod.push(arg2ptr));
   ASSERT_FALSE(nullptr == arg2ptr);

   auto arg3 = make_unique<string>("!");
   auto arg3ptr = arg3.get();
   EXPECT_FALSE(prod.push(arg3ptr));
   EXPECT_FALSE(arg3ptr->empty());
   ASSERT_FALSE(nullptr == arg3ptr);
   EXPECT_EQ("!", *arg3ptr);

   string* receive = nullptr;
   EXPECT_TRUE(cons.pop(receive));
   EXPECT_EQ("hello", *receive);
}

namespace {
   using Ptr = string*;
}

TEST(Queue, FlexibleQ_NoMoveOfPtr) {
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<Ptr>>(2);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   NoMovePtrArgument(producer, consumer);
}

TEST(Queue, FixedQ_NoMoveOfPtr) {
   auto queue = queue_api::CreateQueue<spsc::fixed::circular_fifo<Ptr, 2>>();
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   NoMovePtrArgument(producer, consumer);
}

TEST(Queue, LockedQ_NoMoveUnique) {
   auto queue = queue_api::CreateQueue<mpmc::lock_queue<Ptr>>(2);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto consumer = std::get<queue_api::index::receiver>(queue);
   NoMovePtrArgument(producer, consumer);
}
