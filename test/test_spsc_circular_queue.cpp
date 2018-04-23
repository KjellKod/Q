
#include <gtest/gtest.h>
#include "q/spsc.hpp"
#include <string>

using namespace std;
using FlexibleQ = spsc::flexible::circular_fifo<string>;
using FixedQ = spsc::fixed::circular_fifo<string, 10>;
using FixedSmallQ = spsc::fixed::circular_fifo<string, 3>;


template<typename Q>
void Initialization(Q& q) {
   using namespace spsc;
   using namespace std;
   EXPECT_TRUE(q.empty());
   EXPECT_FALSE(q.full());
   EXPECT_TRUE(q.lock_free());
   EXPECT_EQ(10, q.capacity());
   EXPECT_EQ(10, q.capacity_free());
   EXPECT_EQ(0, q.size());
}


TEST(SPCS_Queue, Initialization) {
   FlexibleQ dQ{10};
   FixedQ fQ{};
   Initialization(fQ);
   Initialization(dQ);
}



template<typename Q>
void AddOne(Q& q) {
   EXPECT_TRUE(q.push("test"));
   EXPECT_FALSE(q.full());
   EXPECT_EQ(10, q.capacity());
   EXPECT_EQ(9, q.capacity_free());
   EXPECT_EQ(1, q.size());
   EXPECT_EQ(1, q.tail());
}

TEST(SPCS_Queue, AddOne) {
   FlexibleQ dQ{10};
   FixedQ fQ{};
   AddOne(fQ);
   AddOne(dQ);
}



template<typename Q>
void AddRemoveOne(Q& q) {
   std::string t = "test";
   EXPECT_TRUE(q.push(t));
   EXPECT_EQ(1, q.tail());
   t.clear();
   EXPECT_TRUE(q.pop(t));
   EXPECT_FALSE(q.full());
   EXPECT_EQ(10, q.capacity());
   EXPECT_EQ(10, q.capacity_free());
   EXPECT_EQ(0, q.size());
   EXPECT_EQ(1, q.tail());
   EXPECT_EQ(1, q.head());
}

TEST(SPCS_Queue, AddRemoveOne) {
   FlexibleQ dQ{10};
   FixedQ fQ{};
   AddRemoveOne(fQ);
   AddRemoveOne(dQ);
}


template<typename Q>
void LoopTillBeginning(Q& q) {
   std::string t = "test";
   for (int i = 0; i < 3; ++i) {

      EXPECT_TRUE(q.push(t));
      EXPECT_EQ(1, q.size());
      EXPECT_EQ(2, q.capacity_free());
      EXPECT_TRUE(q.pop(t));
      EXPECT_EQ(0, q.size());
      EXPECT_EQ(3, q.capacity_free());

      EXPECT_EQ("test", t);
      EXPECT_FALSE(q.full());
      EXPECT_EQ(3, q.capacity());
      EXPECT_EQ(3, q.capacity_free());
      EXPECT_EQ(0, q.size());
   }

   // reached max
   EXPECT_EQ(3, q.tail());
   EXPECT_EQ(3, q.head());
   EXPECT_EQ(3, q.capacity_free());
   EXPECT_EQ(0, q.size());

   // loop till beginning
   EXPECT_TRUE(q.push(t));
   EXPECT_EQ(0, q.tail());
   EXPECT_EQ(3, q.head());

   // head catches up
   EXPECT_TRUE(q.pop(t));
   EXPECT_EQ(0, q.tail());
   EXPECT_EQ(0, q.head());
}


TEST(SPCS_Queue, LoopTillBeginning) {
   FlexibleQ dQ{3};
   FixedSmallQ fQ{};
   LoopTillBeginning(fQ);
   LoopTillBeginning(dQ);
}


template<typename Q>
void Full(Q& q) {
   for (size_t i = 0; i < 10; ++i) {
      EXPECT_FALSE(q.full());
      q.push(to_string(i));
      EXPECT_EQ(10, q.capacity());
      EXPECT_EQ(i + 1, q.size());
      EXPECT_EQ(10 - (i + 1), q.capacity_free());
   }
   EXPECT_EQ(0, q.capacity_free());
   EXPECT_EQ(10, q.size());
}

TEST(SPCS_Queue, Full) {
   FlexibleQ dQ{10};
   FixedQ fQ{};
   Full(fQ);
   Full(dQ);
}

template<typename Q>
void AddTillFullRemoveTillEmpty(Q& q) {
   int size = 0;
   int free = q.capacity();
   const int kMax = free;
   for (size_t i = 0; i < kMax*5; ++i) {
      while (!q.full()) {
         q.push(to_string(i));
         ++size;
         --free;
         EXPECT_EQ(size, q.size());;
      }
      EXPECT_TRUE(q.full());
      EXPECT_EQ(q.size(), q.capacity());
      string t;
      while (!q.empty()) {
         q.pop(t);
         --size;
         ++free;
         EXPECT_EQ(size, q.size());
         EXPECT_EQ(free, q.capacity_free());
      }
   }
}

TEST(SPCS_Queue, AddTillFullRemoveTillEmpty) {
   spsc::fixed::circular_fifo<string, 10> fQ;
   spsc::flexible::circular_fifo<string> dQ(10);
   AddTillFullRemoveTillEmpty(fQ);
   AddTillFullRemoveTillEmpty(dQ);
}




