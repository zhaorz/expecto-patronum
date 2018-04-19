#pragma once

#include <functional>
#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::function;
using std::string;

namespace Game {

  // Basic CPU Voldemort controller
  class Voldemort {

  public:

    Voldemort ()
      : jumpTimeout(jumpInterval)
      , attackTimeout(attackInterval)
    {}

    void setAttack( function<void()> cb ) {
      attackCb = cb;
    }

    void setJump( function<void()> cb ) {
      jumpCb = cb;
    }

    void update( float elapsedTime ) {
      jumpTimeout -= elapsedTime;
      attackTimeout -= elapsedTime;

      if ( jumpTimeout < 0.f ) {
        jumpCb();
        jumpTimeout = jumpInterval;
      }

      if ( attackTimeout < 0.f ) {
        attackCb();
        attackTimeout = attackInterval;
      }
    }

  private:

    static constexpr float jumpInterval = 10.f; // seconds
    float jumpTimeout;

    static constexpr float attackInterval = 2.f; // seconds
    float attackTimeout;

    function<void()> attackCb;
    function<void()> jumpCb;

  };

};
