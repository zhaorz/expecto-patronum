#pragma once

#include <cstdlib>
#include <functional>
#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::function;
using std::string;

double uniformRandom () {
  return static_cast<double>(std::rand()) / RAND_MAX;
}

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
        jumpTimeout = jumpInterval * uniformRandom();
      }

      if ( attackTimeout < 0.f ) {
        attackCb();
        attackTimeout = attackInterval * uniformRandom();
      }
    }

  private:

    static constexpr float jumpInterval = 6.f; // seconds
    static constexpr float attackInterval = 5.f; // seconds

    float jumpTimeout;
    float attackTimeout;

    function<void()> attackCb;
    function<void()> jumpCb;

  };

};
