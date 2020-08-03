#include "graphics/Camera.h"

Camera::Camera(){
  cameraRect.w = graphics->getWindowWidth();
  cameraRect.h = graphics->getWindowHeight();
  cameraRect.y = 0;
  printf("camera itialized %d %d\n", cameraRect.w, cameraRect.h);
}

CameraStateObj Camera::saveState(){
  CameraStateObj stateObj;
  stateObj.camRect = cameraRect;
  stateObj.lowerBound = lowerBound;
  stateObj.upperBound = upperBound;
  stateObj.middle = middle;
  stateObj.yPos = yPos;

  return stateObj;
}

void Camera::loadState(CameraStateObj stateObj){
  cameraRect = stateObj.camRect;
  lowerBound = stateObj.lowerBound;
  upperBound = stateObj.upperBound;
  middle = stateObj.middle;
  yPos = stateObj.yPos;
}

Camera::~Camera(){}

void Camera::update(int p1Xpos, int p2Xpos){
  int midPoint = (p1Xpos + p2Xpos) / 2;
  cameraRect.x = (midPoint - (cameraRect.w/2));
  if(cameraRect.x < 0){
    cameraRect.x = 0;
  }
  if(cameraRect.x + cameraRect.w > 3840){
    cameraRect.x = 3840 - cameraRect.w;
  }

  lowerBound = cameraRect.x;
  upperBound = cameraRect.x+cameraRect.w;
  middle = cameraRect.x + (cameraRect.w/2);
}

void Camera::render(){
  SDL_SetRenderDrawColor(graphics->getRenderer(), 255, 0, 0, 0);
  SDL_RenderDrawRect(graphics->getRenderer(), &cameraRect);
  SDL_SetRenderDrawColor(graphics->getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
}
