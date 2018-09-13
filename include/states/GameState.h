#ifndef _GameState_h
#define _GameState_h

#include <iostream>
#include <vector>
#include "screens/Screen.h"

class GameState{
public:
  virtual ~GameState() { };

  virtual void enter() = 0;
  virtual void exit() = 0;

  void pause() {
    paused = true;
  }
  void resume() {
    paused = false;
  }

  virtual GameState* handleInput(uint16_t inputBits) = 0;
  virtual void update() = 0;
  virtual void draw() = 0;

  Screen* getCurrentScreen() {
    return currentScreen;
  };

  void setCurrentScreen(Screen* screen) {
    currentScreen = screen; 
  };

protected:
  Screen* currentScreen;
  bool paused = false;
};
#endif
