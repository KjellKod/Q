#include "bencharmark_functions.hpp"
# include "Q/q_api.hpp"
#include <gtest/gtest.h>

// namespace {
    
// }

// TEST(Performance, SPSC_Flexible_CircularFifo) {
//    auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(kGoodSizedQueueSize);
//    auto result = RunSPSC(queue, kAmount);
//    EXPECT_EQ(result.size(), 3);
//    auto sent = result[0];
//    auto received = result[1];
//    auto runner = result[2];
//    EXPECT_EQ(sent.sum, received.sum);
//    EXPECT_NE(runner.sum, received.sum);
//    std::cout << "messages / second " << kAmount/(runner.nanotime * 1000000000)
// }