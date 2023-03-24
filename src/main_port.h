#ifndef _MAIN_PORT_H
#define _MAIN_PORT_H

// Code separated from main.cpp to be more portable
int main_port();

// Implemented in platform main.cpp; should call game.tick()
extern void gameloop(void);

#endif // _MAIN_PORT_H
