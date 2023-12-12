#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include "q/q_api.hpp"
#include "q/spsc.hpp"

// Alias for easier readability
using spsc_queue_type = spsc::flexible::circular_fifo<std::string>;
using sender_type = std::shared_ptr<queue_api::Sender<spsc_queue_type>>;
using receiver_type = std::shared_ptr<queue_api::Receiver<spsc_queue_type>>;

void produceMessages(queue_api::Sender<spsc::flexible::circular_fifo<std::string>>& sender, std::atomic<bool>& should_continue_working) {
   std::vector<std::string> greetings = {"Hello", "Bonjour", "Tjena", "Ciao", "Hola", "Hallo", "Hei", "Aloha", "Shalom", "Namaste",
                     "Hello", "Bonjour", "Tjena", "Ciao", "Hola", "Hallo", "Hei", "Aloha", "Shalom", "Namaste"};

   size_t counter = 0;
   for (auto greet : greetings) {
      ++counter;
      while (!sender.push(greet)) {
         std::this_thread::yield();  // Simple strategy: yield and try again
      }
   }
   should_continue_working.store(false);
   std::ostringstream oss;
   oss << __FUNCTION__ << " sent " << counter << " greetings\n";
   std::cout << oss.str();
}

// we can use template signature to make it easier for the input arguments
template<typename ReceiverQ>
void consumeMessages(ReceiverQ& receiver, std::atomic<bool>& keep_working) {

   std::string msg;
   while (keep_working || !receiver.empty()) {
      if (!receiver.pop(msg)) {
         std::this_thread::yield();
      } else {
         std::cout << "Received: " << msg << "\n";
      }
   }
}


int main() {
   // Create a flexible SPSC queue with a dynamic size at runtime
   auto queue = queue_api::CreateQueue<spsc::flexible::circular_fifo<std::string>>(10);

   // Get the sender and receiver endpoints of the queue
   auto senderQ = std::get<queue_api::index::sender>(queue);
   std::atomic<bool> keep_working{true};
   std::thread producer_thread(produceMessages, std::ref(senderQ), std::ref(keep_working));

   auto receiverQ = std::get<queue_api::index::receiver>(queue);
   // similar as above, but showing how template calling can make the thread-function API easier. 
   std::thread consumer_thread(consumeMessages<decltype(receiverQ)>, std::ref(receiverQ), std::ref(keep_working));

   // Wait for both threads to complete
   producer_thread.join();
   consumer_thread.join();
   return 0;
}

