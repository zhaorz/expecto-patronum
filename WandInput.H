#pragma once

#include <iostream>

#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <utility>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "RawInput.H"


using std::lock_guard;
using std::mutex;
using std::pair;
using std::queue;
using std::thread;
using std::unordered_map;
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
      Jump,
      Attack,
      Reflect,
      OutOfScreen,
    };

    // Members
    EventType type;

    union {
      WandPointEvent wandPoint;
    };
  };

  class WandInput {

  public:

    typedef pair<pair<double, double>, pair<double, double>> dxdyRange;

    static const int analysisInterval = 50;    // Milliseconds
    static const int analysisWindow = 500;     // Milliseconds

    static const unordered_map<Event::EventType, dxdyRange> analysisThresholds;

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

      timer.expires_from_now(boost::posix_time::milliseconds(analysisInterval));
      timer.async_wait(boost::bind(&WandInput::analyze, this));

      io.run();
    }

    void analyze() {
      // std::cout << "WandInput::analyze" << std::endl;

      long after = std::chrono::duration_cast<RawInput::ms>(RawInput::Clock::now()
                                                            .time_since_epoch()).count() - analysisWindow;

      if ( buf.size() < 2 ) {
        timer.expires_from_now(boost::posix_time::milliseconds(analysisInterval));
        timer.async_wait(boost::bind(&WandInput::analyze, this));

        return;
      }

      double dx = 0.;
      double dy = 0.;

      double prevX = buf.rbegin()->x;
      double prevY = buf.rbegin()->y;

      for ( auto e = buf.rbegin() + 1; e < buf.rend(); e++ ) {
        if ( e->t < after ) break;

        dx += prevX - e->x;
        dy += prevY - e->y;

        prevX = e->x;
        prevY = e->y;
      }

      // std::cout << "WandInput::analyze : detected : dx,dy = " << dx << ", " << dy << std::endl;

      for ( auto e = analysisThresholds.cbegin(); e != analysisThresholds.cend(); e++ ) {
        dxdyRange range = e->second;

        double dx0, dx1, dy0, dy1;

        std::tie(dx0, dx1) = range.first;
        std::tie(dy0, dy1) = range.second;

        if ( (dx0 < dx && dx < dx1) && (dy0 < dy && dy < dy1) ) {
          std::cout << "WandInput::analyze : triggered : " << e->first << std::endl;

          Event event;
          event.type = e->first;

          pushEvent( event );
          break;
        }
      }

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

  const unordered_map<Event::EventType, WandInput::dxdyRange> WandInput::analysisThresholds = {
    { Event::Jump,    { { -0.1,  0.1 }, { -1.0, -0.3 } }},
    { Event::Attack,  { {  0.3,  1.0 }, { -1.0, -0.3 } }},
    { Event::Reflect, { { -1.0, -0.3 }, {  0.3,  1.0 } }},
  };
}
