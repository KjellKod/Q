# Q
Collection of public domain queue's by KjellKod
All queues are in-process, thread-to-thread queues. 

1. **SPSC:** *single producer, single consumer* 
⋅⋅* Any more threads than 1 on each side will break the thread safety guarantees
⋅⋅* 1 - compiled time fixed in size
⋅⋅* 1 - runtime, at construction, set max size of queue or set to unlimited in size
2. **MPMC:** *multiple producer, multiple consumer*









