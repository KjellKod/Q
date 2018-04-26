
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


TEST(SPCS_CircularQueue, Initialization) {
   FlexibleQ dQ{10};
   FixedQ fQ{};
   Initialization(fQ);
   Initialization(dQ);
}



template<typename Q>
void AddOne(Q& q) {
   std::string arg = "test";
   EXPECT_TRUE(q.push(arg));
   EXPECT_FALSE(q.full());
   EXPECT_EQ(10, q.capacity());
   EXPECT_EQ(9, q.capacity_free());
   EXPECT_EQ(1, q.size());
   EXPECT_EQ(1, q.tail());
}

TEST(SPCS_CircularQueue, AddOne) {
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

TEST(SPCS_CircularQueue, AddRemoveOne) {
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


TEST(SPCS_CircularQueue, LoopTillBeginning) {
   FlexibleQ dQ{3};
   FixedSmallQ fQ{};
   LoopTillBeginning(fQ);
   LoopTillBeginning(dQ);
}


template<typename Q>
void Full(Q& q) {
   for (size_t i = 0; i < 10; ++i) {
      EXPECT_FALSE(q.full());
      std::string number = to_string(i);
      q.push(number);
      EXPECT_EQ(10, q.capacity());
      EXPECT_EQ(i + 1, q.size());
      EXPECT_EQ(10 - (i + 1), q.capacity_free());
   }
   EXPECT_EQ(0, q.capacity_free());
   EXPECT_EQ(10, q.size());
}

TEST(SPCS_CircularQueue, Full) {
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
        std::string number = to_string(i);
        q.push(number);
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

TEST(SPCS_CircularQueue, AddTillFullRemoveTillEmpty) {
   spsc::fixed::circular_fifo<string, 10> fQ;
   spsc::flexible::circular_fifo<string> dQ(10);
   AddTillFullRemoveTillEmpty(fQ);
   AddTillFullRemoveTillEmpty(dQ);
}












