#ifndef _Screen_h
#define _Screen_h 

#include <vector>
#include <SDL_image.h>
#include "graphics/GameTexture.h"
#include "graphics/Graphics.h"

// Base class for all game screens. 
// init method & update method
// up here to abstract general stuff out of screen subclasses?
// idk yet
class Screen {
// a screen should be the final surface thats written to for a current frame
// subclass of screen will basically represent the games current screen state
// so in the main class I can do something like Screen currentScreen = new OpeningScreen()
public:
  virtual void init() = 0;
  virtual void update() = 0;
  virtual void draw() = 0;

  virtual void addTexture(GameTexture* gText){
    textureList.push_back(gText);
  };

  virtual void removeTexture(int index){
    textureList.erase(textureList.begin() + index);
  };

protected:
  Graphics* graphics = Graphics::getInstance();
  std::vector<GameTexture*> textureList;
};
#endif
