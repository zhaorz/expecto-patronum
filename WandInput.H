#pragma once

#include <iostream>

#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "RawInput.H"


using std::lock_guard;
using std::mutex;
using std::queue;
using std::thread;
using namespace std::placeholders;  // for _1, _2, _3...


namespace Wand {

  enum Event {
    Jump,
  };

  class WandInput {

  public:

    WandInput() : eventQueue(), rawInput() {
      std::cout << "WandInput : initializing ..." << std::endl;

      RawInput::InputCb cb = std::bind(&WandInput::rawInputCb, this, _1, _2, _3);
      rawInput.registerCallback(cb);

      rawInputThread = thread([&] () { rawInput.run(); });

      std::cout << "WandInput : initialized" << std::endl;
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

    void rawInputCb (double x, double y, double t) {
      std::cout << "WandInput : " << x << ", " << y << ", t = " << t << std::endl;
      return;
    };

  private:

    void pushEvent( Event event ) {
      lock_guard<mutex> guard(eventQueueMutex);

      eventQueue.push(event);
    }

    mutex eventQueueMutex;

    // TODO: Add locks
    queue<Event> eventQueue;

    RawInput rawInput;
    thread rawInputThread;

  };

}
