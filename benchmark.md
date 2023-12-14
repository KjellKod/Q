# Scalability Benchmark 

## Throughput messages / second 
N producer threads push a 4-byte integer onto a queue. M consumer threads op the integers from the queue. All producers posts 1,000,000 messages. The total time to send and receive all messages is measured. The test is repeated 20 times. 

Maximum throughput, minimum throughput and mean throughput is shown in the graph. 
Average time per ops (push/pop) is shown below. 

# Thoughts on performance measurements. 
As long as you have a good understanding of a queue's performancce metrics it's near useless to rely on someone elses performances. Optimizing queue size, avoiding talking about thread_affinity manipulation (or lack thereof), avoiding worst case scenarios etc, etc are all blatantly common in the queue measurements that I've seen. 

If you are interested in verifying Q vs something else, then think about what scenarios the queue should perform well at. Do your own performance testing with the queue alternatives on the platform!!! were you intend to use it. 

Maybe you have to put a wrapper of sorts (like [q_api](src/q/q_api.hpp)) so you can exchange the queues but it's well worth the effort. Most likely your system will be doing a lot of other tasks, and hyper CPU intensive [spin locks](https://probablydance.com/2019/12/30/measuring-mutexes-spinlocks-and-how-bad-the-linux-scheduler-really-is/) and platform specific CPU instructions might very well be impacting negatively on other performance sensitive tasks of your software. 

# Thoughts on correctness 
There are some really amazing C++ efficiency queues out there. Most of which are crazy hard to reason about if you read the code and for that reason almost impossible to know if they are actually correct. 

The Q library aims to have the queue logics to be extremely easy to understand, possibly with some infinitesimal performance degradation compared to near-impossible-to-reason-about queues. 

he Q library is built on common building blocks and at it's core has easy to understand logic, the famous [lock-free-circular-fifo](https://kjellkod.wordpress.com/2012/11/28/c-debt-paid-in-full-wait-free-lock-free-queue/). I wrote about this in 2012 and with some minor changes it's pretty much the same root building block as then. It's nothing new about this queue, in fact there are a myriad of similar queues out there. 