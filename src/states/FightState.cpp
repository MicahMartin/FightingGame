#include "states/FightState.h"
#include "screens/FightScreen.h"
#include "game_objects/Character.h"
#include "game_objects/Stage.h"

FightState::FightState(){ 
  printf("creating new fightState\n");
  stateName = "FightState";
}

FightState::~FightState(){ 
  printf("in fightState destructor\n");
}


void FightState::enter(){ 
  printf("entering fightState\n");
  // init all fields
  charStateManager->reset();
  graphics->setCamera(&camera);
  camera.update(1700, 2200);
  player1 = new Character(std::make_pair(1700,0), 1);
  player2 = new Character(std::make_pair(2200,0), 2);
  player1->virtualController = inputManager->getVirtualController(0);
  player1->virtualController->initCommandCompiler();
  player2->virtualController = inputManager->getVirtualController(1);
  player2->virtualController->initCommandCompiler();

  player1->otherChar = player2;
  player2->otherChar = player1;
 
  charStateManager->registerCharacter(player1, 1);
  charStateManager->registerCharacter(player2, 2);

  player1->init();
  player2->init();
}

void FightState::exit(){ 
  printf("exiting fight state\n");
  delete player1;
  delete player2;
  delete this;
}

void FightState::pause(){ }
void FightState::resume(){ }

void FightState::handleInput(){ 
  updateFaceRight();
  checkHealth();
  if (charStateManager->screenFrozen == false) {
    player1->handleInput();
    player2->handleInput();
  }
}

void FightState::update(){ 
  stateTime++;
  if(player1->getPos().first - player1->width <= 0 || player1->getPos().first + player1->width >= 3840){
    player1->inCorner = true;
  } else {
    player1->inCorner = false;
  }

  if(player2->getPos().first - player2->width <= 0 || player2->getPos().first + player2->width >= 3840){
    player2->inCorner = true;
  } else {
    player2->inCorner = false;
  }

  checkHitCollisions();

  if(charStateManager->screenFrozen && --screenFreeze == 0){
    charStateManager->screenFrozen = false;
  }

  if(charStateManager->screenFrozen == false){
    player1->update();
    player2->update();
  } else {
    charStateManager->screenFreezeTime = screenFreeze;
    player1->currentState->handleCancels();
    player2->currentState->handleCancels();
  }
  checkPushCollisions();

  checkBounds();
  camera.update(player1->getPos().first, player2->getPos().first);
  printf("player1Pos:%d, player2Pos:%d \n", player1->getPos().first, player2->getPos().first);
}

void FightState::draw(){  
  double screenDrawStart, screenDrawEnd,
         barDrawStart, barDrawEnd,
         p1DrawStart, p1DrawEnd,
         p2DrawStart, p2DrawEnd;
  screenDrawStart = SDL_GetTicks();
  currentScreen.draw();
  screenDrawEnd = SDL_GetTicks();
  // TODO: move renderHP into currentScreen
  barDrawStart = SDL_GetTicks();
  renderHealthBars();
  barDrawEnd = SDL_GetTicks();
  renderComboCount();
  renderInputHistory();
  if (player1->frameLastAttackConnected > player2->frameLastAttackConnected) {
    p2DrawStart = SDL_GetTicks();
    player2->draw();
    p2DrawEnd = SDL_GetTicks();

    p1DrawStart = SDL_GetTicks();
    player1->draw();
    p1DrawEnd = SDL_GetTicks();
  } else {
    p1DrawStart = SDL_GetTicks();
    player1->draw();
    p1DrawEnd = SDL_GetTicks();

    p2DrawStart = SDL_GetTicks();
    player2->draw();
    p2DrawEnd = SDL_GetTicks();
  }
   double screenDraw = screenDrawEnd - screenDrawStart;
   double barDraw = barDrawEnd - barDrawStart;
   double p1Draw = p1DrawEnd - p1DrawStart;
   double p2Draw = p2DrawEnd - p2DrawStart;
   if (screenDraw > 0) {
     // printf("screenDraw %f\n", screenDraw);
   }
   if (barDraw > 0) {
     // printf("barDraw %f\n", barDraw);
   }
   if(p1Draw > 0) {
     // printf("p1Draw %f\n", p1Draw);
   }
   if(p2Draw > 0){
     // printf("p2Draw %f\n", p2Draw);
   }
}

void FightState::checkPushCollisions(){
  // get the collision box(s) for the current state
  std::pair<int, int> p1Pos = player1->getPos();
  std::pair<int, int> p2Pos = player2->getPos();

  for (auto p1PushBox : player1->currentState->pushBoxes) {
    if(!p1PushBox->disabled){
      for (auto p2PushBox : player2->currentState->pushBoxes) {
        if(!p2PushBox->disabled){
          if (CollisionBox::checkAABB(*p1PushBox, *p2PushBox)) {
            // find how deeply intersected they are
            bool p1Lefter = p1Pos.first < p2Pos.first;

            if(p1Lefter){
              int p1RightEdge = p1PushBox->positionX + p1PushBox->width;
              int p2LeftEdge = p2PushBox->positionX;
              int depth = p1RightEdge - p2LeftEdge;

              // account for over bound 
              if ((p2Pos.first+player2->width) + (depth/2) > 3840) {
                int remainder = 3840 - (p2Pos.first + (depth/2));
                player2->setXPos(3840);
                player1->setX(-depth);
              } else if ((p1Pos.first - player1->width) - (depth/2) < 0){
                int remainder = p1Pos.first + (depth/2);
                player1->setXPos(0);
                player2->setX(depth);
              } else {
                player2->setX(depth/2);
                player1->setX(-depth/2);
              }
            } else {
              int p2RightEdge = p2PushBox->positionX + p2PushBox->width;
              int p1LeftEdge = p1PushBox->positionX;
              int depth = p2RightEdge - p1LeftEdge;

              // account for over bound 
              if ((p1Pos.first+player1->width) + (depth/2) > 3840) {
                int remainder = 3840 - (p1Pos.first + (depth/2));
                player1->setXPos(3840);
                player2->setX(-depth);
              } else if ((p2Pos.first - player2->width) - (depth/2) < 0){
                int remainder = p2Pos.first + (depth/2);
                player2->setXPos(0);
                player1->setX(depth);
              } else {
                player2->setX(-depth/2);
                player1->setX(depth/2);
              }
            }
            player1->updateCollisionBoxPositions();
            player2->updateCollisionBoxPositions();
          }
        }
      }
    }
  }
}

void FightState::checkHitCollisions(){
  if (!player1->currentState->hitboxesDisabled) {
    for (auto p1Hitbox : player1->currentState->hitBoxes) {
      if(!p1Hitbox->disabled){
        for (auto p2HurtBox : player2->currentState->hurtBoxes) {
          if(!p2HurtBox->disabled){
            if (CollisionBox::checkAABB(*p1Hitbox, *p2HurtBox)) {
              player1->frameLastAttackConnected = gameTime; 
              printf("hitbox collision detected\n");
              // TODO: Run hitscript
              charStateManager->screenFrozen = true;
              screenFreeze = p1Hitbox->hitstop;
              charStateManager->screenFreezeTime = p1Hitbox->hitstop;
              player1->currentState->hitboxesDisabled = true;

              player2->control = 0;
              player2->health -= p1Hitbox->damage;
              player2->hitstun = p1Hitbox->hitstun;
              if (player2->inCorner) {
                player1->pushTime = p1Hitbox->pushTime;
                player1->_negVelSetX(p1Hitbox->pushback);
              } else {
                player2->pushTime = p1Hitbox->pushTime;
                player2->_negVelSetX(p1Hitbox->pushback);
              }
              player2->comboCounter++;
              if(p1Hitbox->canTrip){
                player2->changeState(24);
              } else {
                player2->changeState(9);
              }
              if(player2->comboCounter > 1){
                printf("player 2 been combo'd for %d hits\n", player2->comboCounter);
              }
            }
          }
        }
      }
    }
  }
}

void FightState::checkBounds(){
  if(player1->getPos().first - player1->width <= 0) {
    player1->setXPos(0+player1->width);
    player1->updateCollisionBoxPositions();
  }
  if (player1->getPos().first - player1->width < camera.lowerBound) {
    printf("why am I outside the camera on the left?\n");
    player1->setXPos(camera.lowerBound+player1->width);
    player1->updateCollisionBoxPositions();
  }

  if(player1->getPos().first + player1->width >= 3840) {
    player1->setXPos(3840 - player1->width);
    player1->updateCollisionBoxPositions();
  }
  if (player1->getPos().first + player1->width > camera.upperBound) {
    printf("why am I outside the camera on the right?\n");
    player1->setXPos(camera.upperBound - player1->width);
    player1->updateCollisionBoxPositions();
  }

  if(player2->getPos().first - player2->width <= 0) {
    player2->setXPos(0 + player2->width);
    player2->updateCollisionBoxPositions();
  }
  if (player2->getPos().first - player2->width < camera.lowerBound) {
    player2->setXPos(camera.lowerBound + player2->width);
    player2->updateCollisionBoxPositions();
  }

  if(player2->getPos().first + player2->width >= 3840) {
    player2->setXPos(3840 - player2->width);
    player2->updateCollisionBoxPositions();
  }
  if (player2->getPos().first + player2->width > camera.upperBound) {
    player2->setXPos(camera.upperBound - player2->width);
    player2->updateCollisionBoxPositions();
  }
}

void FightState::checkHealth(){
  if (player1->health <= 0 || player2->health <= 0) {
    stateManager->popState();
  }
}

void FightState::updateFaceRight(){
  if(player1->getPos().first <= player2->getPos().first){
    if(!player1->currentState->checkFlag(NO_TURN)){
      player1->faceRight = true;
    }
    player1->inputFaceRight = true;
    if(!player2->currentState->checkFlag(NO_TURN)){
      player2->faceRight = false;
    }
    player2->inputFaceRight = true;
  } else {
    if(!player1->currentState->checkFlag(NO_TURN)){
      player1->faceRight = false;
    }
      player1->inputFaceRight = false;
    if(!player2->currentState->checkFlag(NO_TURN)){
      player2->faceRight = true;
    }
      player2->inputFaceRight = true;
  }
}

void FightState::renderHealthBars(){
  int p1Hp = player1->health;
  float p1HpPercent = p1Hp / player1->maxHealth;
  int p2Hp = player2->health;
  float p2HpPercent = (float)p2Hp / (float)player2->maxHealth;

  SDL_Color green = {0, 255, 0, 0};
  SDL_Color red = {255, 0, 0, 0};
  // draw p1 healthbar
  currentScreen.renderHealthBar(100, 50, 500, 50, p1HpPercent, green, red);
  currentScreen.renderHealthBar(680, 50, 500, 50, p2HpPercent, green, red);
}

void FightState::renderComboCount(){
  int p1ComboCount = player1->comboCounter;
  int p2ComboCount = player2->comboCounter;
  if(p2ComboCount > 0){
    currentScreen.renderComboCount(true, p2ComboCount);
  }
  if(p1ComboCount > 0){
    currentScreen.renderComboCount(false, p1ComboCount);
  }
}

void FightState::renderInputHistory(){
  // printf("p1InputEventList size %ld\n", player1->virtualController->inputEventList.size());
  currentScreen.renderInputHistory(true, player1->virtualController->inputEventList);
  currentScreen.renderInputHistory(false, player2->virtualController->inputEventList);
}
