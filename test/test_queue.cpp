#include <gtest/gtest.h>
#include "q/spsc.hpp"
#include "q/q_api.hpp"
#include <string>

using namespace std;
using Type = string;
using FlexibleQ = spsc::flexible::circular_fifo<Type>;
using FixedQ = spsc::fixed::circular_fifo<Type, 10>;



template<typename Prod, typename Cons>
void ProdConsInitialization(Prod& prod, Cons& cons ) {
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
   auto queue = CreateQueue<FlexibleQ>(10);
   auto producer = std::get<Queue<Type>::ProducerIndex>(queue);
   auto consumer = std::get<Queue<Type>::ConsumerIndex>(queue);
}


template<typename Prod, typename Cons>
void QAddOne(Prod& prod, Cons& cons) {
   EXPECT_TRUE(prod.push("test"));
   EXPECT_FALSE(prod.full());
   EXPECT_EQ(10, prod.capacity());
   EXPECT_EQ(9, prod.capacity_free());
   EXPECT_EQ(1, prod.size());
   EXPECT_EQ(1, prod.tail());
}

TEST(SPCS_CIRCULAR_QUEUE, QAddOne) {
   FlexibleQ dQ{10};
   FixedQ fQ{};
   QAddOne(dQ, fQ);

}


template<typename Prod, typename Cons>
void AddTillFullRemoveTillEmpty(Prod& prod, Cons& cons) {
   int size = 0;
   int free = prod._q->capacity();
   int maxSize = free * 5;

   for (size_t i = 0; i < maxSize; ++i) {
      while (!prod.full()) {
         prod.push(to_string(i));
         ++size;
         --free;
         EXPECT_EQ(size, prod.size());
         EXPECT_EQ(free, prod.capacity_free());
      }
      EXPECT_TRUE(prod.full());
      EXPECT_TRUE(cons.full());
      EXPECT_EQ(prod.size(), prod.capacity());
      EXPECT_EQ(cons.size(), cons.capacity());

      string t;
      while (!cons.empty()) {
         cons.pop(t);
         --size;
         ++free;
         EXPECT_EQ(size, cons.size());
         EXPECT_EQ(free, cons.capacity_free());
      }
   }
}



TEST(Queue, ADdTillFullRemoveTillEmpty) {
   auto queue = CreateQueue<FlexibleQ>(2);
   auto producer = std::get<Queue<Type>::ProducerIndex>(queue);
   auto consumer = std::get<Queue<Type>::ConsumerIndex>(queue);
   AddTillFullRemoveTillEmpty(producer, consumer);
}

