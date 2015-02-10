#ifndef AUDIO_CTL
#define AUDIO_CTL
#include "Arduino.h"
#include <Adafruit_VS1053.h>

enum cmdType {
  LOOP,
  BUTTON_PRESS,
  POT_SELECT
};

class audioCmd {
public:
  //char track[16];
  String track[10];
  int numTracks;
  int curTrack;
  int nextTrack;
  cmdType type;
  bool invert;
  bool pressed;
  
  long lTimer;
  bool elapsed;
  
  int input;
  audioCmd();
  void execute();
  void play(int);
  bool debounce();
};

class audioControl {
public:
  int volEnc;
  int volPot;
  //String curLine;
  int numCmds;
  audioCmd * cmds[5];
  char buffer[32];
  int line_number;
  int loopStart;
  
  audioControl(void);
  void setup(Adafruit_VS1053_FilePlayer *);
  void trigger();
  
  void idle();
  
};
#endif
