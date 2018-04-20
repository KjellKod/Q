
#include <gtest/gtest.h>
#include "q/spsc_circular_fifo.hpp"
#include <string>


TEST(SPCS_CIRCULAR_QUEUE, Initialization) {
   using namespace spsc;
   using namespace std;
   circular_fifo<string> q{10};
   EXPECT_TRUE(q.is_empty());
   EXPECT_FALSE(q.is_full());
   EXPECT_TRUE(q.is_lock_free());
}