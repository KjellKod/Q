# Q
Collection of public domain queue's by KjellKod
All queues are in-process, thread-to-thread queues. 

# SPSC - **single producer, single consumer**

This lock-free queue is safe to use between one producer thread and one consumer thread. 
**SPSC lock-free options:**
1. `fixed_circular_fifo`: Set the size of the queue in your code, the size is set during compiled time.
1. `flexible_circular_fifo`: Set the size of the queue in the constructor.

_The SPSC is a powerful building block from which you can create more lock-free complicated queue structures if number of producers and consumers are known at creation time._ 

##### SPSC Naive Example

The raw and "unsafe" way to create the queue is to just "create it".  This makes it harder to use right as the producer and consumer threads  must only touch "their" parts of the API or the queue would not be thread-safe. 
```
using spsc_queue_type = spsc::flexible::circular_fifo<string>;
auto q_size = 1000;
auto spsc_queue = spsc_queue_type(q_size); 
//  through spsc_queue the FULL producer/consumer API is available, 
//  which can be dangerous
```

## A better example
A better way to use the **spsc** queue is through the [q/q_api.hpp](src/q/q_api.hpp). A constructor method will generate the queue that is embedded in a `std::pair` with template-enforced facade which only allows the safe APIs for the **p** (producer) and **c** (consumer) specific APIs. 

This makes it easy and safe, as you share only the **p** or **c** part with a dedicated thread. See [q/q_api.hpp](src/q/q_api.hpp) for details on how this is implemented. 

```
// somewhere
template<typename T>
void sendData(T producer) { ..... }

// somewhere
template<typename T>
void receiveData(T consumer) { ..... }

auto queue = queue_api::CreateQueue<spsc_queue_type>(q_size);
auto senderQ = std::get<queue_api::index::sender>(queue);
std::thread producer_thread(sendData, senderQ);

auto receiverQ = std::get<queue_api::index::receiver>(queue);
std::thread consumer_thread(receiveData, receiverQ);
...
// producer_thread.join();
// consumer_tread.join();
```
See a runnable `std::async` example at [examples/spsc_main.cpp](examples/spsc_main.cpp)


## SPSC API
From [queue_api::Base<QType>](src/q/q_api.hpp) (see [Curiously recurring template pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)) we have 
```
     // qref is the actual queue instance, regardless of type. 
      bool empty() const;
      bool full() const;
      size_t capacity() const;
      size_t capacity_free() const;
      size_t size() const;
      bool lock_free() const;
      size_t usage() const;å
```

From [queue_api::Sender: public Base<QType>](https://github.com/KjellKod/Q/blob/master/src/q/q_api.hpp#L79)
    
```
    bool push(Element& item);
```

From [queue_api::Receiver: public Base<QType>](https://github.com/KjellKod/Q/blob/master/src/q/q_api.hpp#L137)
    
```
    bool pop(Element& item);
    bool wait_and_pop(Element& item, const milliseconds wait_ms) { return sfinae::wait_and_pop(... }
```

## SPSC Usage
Please see the [examples/spsc_main.cpp](examples/spsc_main.cpp) for example usage. 


# NOT YET DOCUMENTED API

2. **MPMC:** *multiple producer, multiple consumer*
    - `lock flexible-lock-queue`: runtime, at construction, set max size of queue or set to unlimited in size
3. **MPSC:** *multiple producer, singe consumer*
    - `flexible or fixed lock-free circular fifo`: Using fair scheduling the many SPSC queues are consumed in an optimized round-robin manner
4. **SPMC:** *single producer, multiple consumer*
    - `flexible or fixed lock-free circular fifo`: Using fair scheduling the producer transfers over many SPSC queues







