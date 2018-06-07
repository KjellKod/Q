# Q
Collection of public domain queue's by KjellKod
All queues are in-process, thread-to-thread queues. 

1. **SPSC:** *single producer, single consumer* 
    - Any more threads than 1 on each side will break the thread safety guarantees for the *SPSC* queues
    - `lock-free fixed_circular_fifo`: compiled time fixed in size
    - `lock-free flexible_circular_fifo`: runtime, at construction, set max size of queue or set to unlimited in size
2. **MPMC:** *multiple producer, multiple consumer*
    - `lock flexible-lock-queue`: runtime, at construction, set max size of queue or set to unlimited in size
3. **MPSC:** *multiple producer, singe consumer*
    - `flexible or fixed lock-free circular fifo`: Using fair scheduling the many SPSC queues are consumed in an optimized round-robin manner
4. **SPMC:** *single producer, multiple consumer*
    - `flexible or fixed lock-free circular fifo`: Using fair scheduling the producer transfers over many SPSC queues







