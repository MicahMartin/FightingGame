#ifndef _FightState_h
#define _FightState_h

#include "states/GameState.h"
#include "game_objects/Stage.h"
#include "game_objects/Character.h"

class FightState : public GameState{
public:
  FightState();
  ~FightState();

  void enter();
  void exit();

  void pause();
  void resume();

  void handleInput(VirtualController* vc);
  void update();
  void draw();

private:
  Character* player1;
  Character* player2;
  Stage* stage;
};
#endif