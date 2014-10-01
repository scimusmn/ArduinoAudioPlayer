#ifndef AUDIO_CTL
#define AUDIO_CTL
#include "Arduino.h"
#include <SFEMP3Shield.h>

enum cmdType {
  LOOP,
  BUTTON_PRESS,
  POT_SELECT
};

class audioCmd {
public:
  char track[16];
  cmdType type;
  bool invert;
  bool pressed;
  
  long lTimer;
  bool elapsed;
  
  int input;
  audioCmd();
  void execute();
  bool debounce();
};

class audioControl {
public:
  int volEnc;
  int volPot;
  String curLine;
  int numCmds;
  audioCmd * cmds[5];
  char buffer[32];
  int line_number;
  int loopStart;
  
  audioControl(void);
  void setup(SFEMP3Shield *);
  
  void idle();
  
};
#endif
