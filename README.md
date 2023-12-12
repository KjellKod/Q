# Q
Collection of public domain queue's by KjellKod
All queues are in-process, thread-to-thread queues. 

# SPSC - **single producer, single consumer**

This lock-free queue is safe to use between one producer thread and one consumer thread. 
**SPSC lock-free options:**
1. `fixed_circular_fifo`: Set the size of the queue in your code, the size is set during compiled time.
1. `flexible_circular_fifo`: Set the size of the queue in the constructor.

_The SPSC is a powerful building block from which you can create more lock-free complicated queue structures if number of producers and consumers are known at creation time._ 

Please see [spsc documentation](spsc.md) for details. 



# NOT YET DOCUMENTED API

2. **MPMC:** *multiple producer, multiple consumer*
    - `lock flexible-lock-queue`: runtime, at construction, set max size of queue or set to unlimited in size
3. **MPSC:** *multiple producer, singe consumer*
    - `flexible or fixed lock-free circular fifo`: Using fair scheduling the many SPSC queues are consumed in an optimized round-robin manner
4. **SPMC:** *single producer, multiple consumer*
    - `flexible or fixed lock-free circular fifo`: Using fair scheduling the producer transfers over many SPSC queues







