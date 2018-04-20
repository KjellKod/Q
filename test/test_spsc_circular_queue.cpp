
#include <gtest/gtest.h>
#include "q/spsc_circular_fifo.hpp"
#include <string>


TEST(SPCS_CIRCULAR_QUEUE, Initialization) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{10};
   EXPECT_TRUE(q.empty());
   EXPECT_FALSE(q.full());
   EXPECT_TRUE(q.lock_free());
   EXPECT_EQ(10, q.capacity());
   EXPECT_EQ(10, q.capacity_free());
   EXPECT_EQ(0, q.size());
}

TEST(SPCS_CIRCULAR_QUEUE, AddOne) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{10};
   EXPECT_TRUE(q.push("test"));
   EXPECT_FALSE(q.full());
   EXPECT_EQ(10, q.capacity());
   EXPECT_EQ(9, q.capacity_free());
   EXPECT_EQ(1, q.size());
   EXPECT_EQ(1, q.tail());
}

TEST(SPCS_CIRCULAR_QUEUE, AddRemoveOne) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{10};
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

TEST(SPCS_CIRCULAR_QUEUE, LoopTillBeginning) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{3};
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


TEST(SPCS_CIRCULAR_QUEUE, Full) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{10};
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



TEST(SPCS_CIRCULAR_QUEUE, AddTillFullRemoveTillEmpty) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{2};
   int size = 0;
   int free = 2;
   for (size_t i = 0; i < 10; ++i) {
      while (!q.full()) {
         q.push(to_string(i));
         ++size;
         --free;
         EXPECT_EQ(size, q.size());
         EXPECT_EQ(free, q.capacity_free());
      }
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



