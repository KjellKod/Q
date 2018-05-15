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
#include "q/mpsc_flexible_roundrobin.hpp"
#include "q/spsc_flexible_circular_fifo.hpp"
#include "q/q_api.hpp"

TEST(MPSC, CreateOneQueue) {
   using element = std::string;
   using qtype = spsc::flexible::circular_fifo<element>;
   auto queue = queue_api::CreateQueue<qtype>(10);
   auto producer = std::get<queue_api::index::sender>(queue);
   auto temporary = std::get<queue_api::index::receiver>(queue);

   // convert the setup to a MPSC setup
   mpsc::roundrobin_receiver<qtype> consumer({temporary});

   std::string e;
   EXPECT_FALSE(consumer.pop(e));
   EXPECT_TRUE(consumer.empty());
   EXPECT_FALSE(consumer.full());
   EXPECT_EQ(10, consumer.capacity());
   EXPECT_EQ(10, consumer.capacity_free());
   EXPECT_EQ(0, consumer.size());
   EXPECT_TRUE(consumer.lock_free());
}

//  test    size_t increment(size_t idx) const;
