#pragma once

#include <iostream>

#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "RawInput.H"


using std::lock_guard;
using std::mutex;
using std::queue;
using std::thread;
namespace ph = std::placeholders;


namespace Wand {

  class Event {
  public:

    struct WandPointEvent {
      double x;
      double y;
    };

    enum EventType {
      WandPoint,
    };

    // Members
    EventType type;

    union {
      WandPointEvent wandPoint;
    };
  };

  class WandInput {

  public:

    static const int analysisInterval = 500;    // Milliseconds
    static const int analysisWindow = 500;     // Milliseconds

    WandInput() : eventQueue()
                , rawInput()
                , buf(maxBuf)
                , io()
                , timer(io)
    {
      std::cout << "WandInput : initializing ..." << std::endl;

      RawInput::InputCb cb = std::bind(&WandInput::rawInputCb, this, ph::_1, ph::_2, ph::_3);
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

      timer.expires_from_now(boost::posix_time::milliseconds(analysisInterval));
      timer.async_wait(boost::bind(&WandInput::analyze, this));

      io.run();
    }

    void analyze() {
      std::cout << "WandInput::analyze" << std::endl;

      timer.expires_from_now(boost::posix_time::milliseconds(analysisInterval));
      timer.async_wait(boost::bind(&WandInput::analyze, this));
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

    void rawInputCb (double x, double y, long t) {
      Event event;
      event.type = Event::WandPoint;
      event.wandPoint = { x, y };

      pushEvent( event );

      buf.push_back({ x, y, t });

      return;
    };

  private:

    void pushEvent( Event event ) {
      lock_guard<mutex> guard(eventQueueMutex);

      eventQueue.push(event);
    }

    mutex eventQueueMutex;

    queue<Event> eventQueue;

    // Stores rawInput points
    boost::circular_buffer<RawInputEvent> buf;
    static const int maxBuf = 128;

    RawInput rawInput;
    thread rawInputThread;

    // Timer utils
    boost::asio::io_service io;
    boost::asio::deadline_timer timer;

  };

}
