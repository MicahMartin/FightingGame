#include "states/ConfigState.h"

ConfigState::ConfigState(){ }

ConfigState::~ConfigState(){ }


void ConfigState::enter(){ };
void ConfigState::exit(){ };

void ConfigState::pause(){ };
void ConfigState::resume(){ };
void ConfigState::handleInput(VirtualController* vc){  };
void ConfigState::update(){  };
void ConfigState::draw(){  };