#pragma once

#include <iostream>

#include <chrono>
#include <mutex>
#include <queue>
#include <thread>


using std::lock_guard;
using std::mutex;
using std::queue;


namespace Wand {

  enum Event {
    Jump,
  };

  class WandInput {

  public:

    WandInput() : eventQueue() {
      std::cout << "WandInput : initializing ..." << std::endl;
    }

    ~WandInput() {
      std::cout << "WandInput : cleaning up ..." << std::endl;
    }

    void run() {
      std::cout << "WandInput::run" << std::endl;
      // TODO: Start running

      std::this_thread::sleep_for(std::chrono::seconds(2));

      std::cout << "WandInput::run : dispatching event ..." << std::endl;

      pushEvent( Jump );
    }

    bool pollEvent( Event& event ) {
      if ( eventQueue.empty() ) {
        return false;
      }

      lock_guard<mutex> guard(eventQueueMutex);

      event = eventQueue.front();
      eventQueue.pop();

      return true;
    }

  private:

    void pushEvent( Event event ) {
      lock_guard<mutex> guard(eventQueueMutex);

      eventQueue.push(event);
    }

    mutex eventQueueMutex;

    // TODO: Add locks
    queue<Event> eventQueue;

  };

}
