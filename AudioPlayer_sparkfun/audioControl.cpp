#include "audioControl.h"
#include "Arduino.h"
#include <SdFat.h>
#include <SFEMP3Shield.h>

ArduinoOutStream cout(Serial);

static bool audioInterrupt = false;
static bool once = false;
static bool hold = false;
static bool playing = false;
static bool stopOnRelease = false;
static int debounceTime =0;

SFEMP3Shield * mp3 = 0;

audioCmd::audioCmd(){
  input=0;
  lTimer =0;
  invert = false;
  pressed =false;
}

void audioCmd::execute(){
  if(type==LOOP){
    if(!mp3->isPlaying()) mp3->playMP3(track);
  }
  else if(type==BUTTON_PRESS){
    bool bRead = digitalRead(input);
    if(!invert) bRead = !bRead;
    
    if(!bRead){
      pressed = false;
      if(stopOnRelease) mp3->stopTrack();
    }
    else if(!pressed&&bRead) lTimer = debounceTime+millis(), elapsed = false,pressed=true;
    
    if(lTimer<millis()&&!elapsed&& pressed&&!once) lTimer = debounceTime+millis(), elapsed = false;
    
    if(debounce()){
      if(audioInterrupt||(!audioInterrupt&&!mp3->isPlaying())){
        if(bRead) mp3->stopTrack(),mp3->playMP3(track),playing=true;
      }
    }
  }
}

bool audioCmd::debounce(){
  bool ret = lTimer<=millis()&&!elapsed;
  elapsed = lTimer<=millis();
  
  return ret;
}

audioControl::audioControl(void){
    volEnc = -1;
    volPot = -1;
    line_number = 0;
    loopStart=0;
    numCmds=0;
  }

void audioControl::setup(SFEMP3Shield * dMP3){
    mp3 = dMP3;
    ifstream sdin("config.txt");
    int line_number = 0;
  
    while (sdin.getline(buffer, 32, '\n') || sdin.gcount()) {
      int count = sdin.gcount();
      if (sdin.fail()) {
        sdin.clear(sdin.rdstate() & ~ios_base::failbit);
      } else if (sdin.eof()) {
      } else count--;
      
      curLine="";
      for(int j=0; j<32; j++){
        curLine += String(buffer[j]);
      }
      cout << buffer << endl;
      if(curLine.indexOf("volEncoder")>=0) volEnc = curLine.substring(curLine.indexOf('=')+1).toInt();
      else if(curLine.indexOf("volPot")>=0) volPot = curLine.substring(curLine.indexOf('=')+1).toInt();
      else if(curLine.indexOf("interrupt")>=0) audioInterrupt = curLine.substring(curLine.indexOf('=')+1).toInt();
      else if(curLine.indexOf("debounce")>=0) debounceTime = curLine.substring(curLine.indexOf('=')+1).toInt();
      else if(curLine.indexOf("once")>=0) once = curLine.substring(curLine.indexOf('=')+1).toInt(),cout << "once" << endl;
      else if(curLine.indexOf("stopOnRelease")>=0) stopOnRelease = curLine.substring(curLine.indexOf('=')+1).toInt();
      else if(curLine.indexOf("loop")>=0){
        String track = curLine.substring(curLine.indexOf('=')+1);
        cmds[numCmds] = new audioCmd();
        cmds[numCmds]->type=LOOP;
        for(int j=0; j<16; j++){
          cmds[numCmds]->track[j] = track[j];
        }
        cout << cmds[numCmds]->track << endl;
        numCmds++;
      }
      else if(curLine.indexOf("pushButton")>=0){
        String track = curLine.substring(curLine.indexOf('=')+1);
        cmds[numCmds] = new audioCmd();
        cmds[numCmds]->type=BUTTON_PRESS;
        if(curLine.indexOf("!")>=0) cmds[numCmds]->invert=true,cout << "Invert" << endl;
        cmds[numCmds]->input=curLine.substring(curLine.indexOf('[')+1,curLine.indexOf(']')).toInt()+14;
        cout << "number is " << cmds[numCmds]->input << endl;
        pinMode(cmds[numCmds]->input,INPUT_PULLUP);
        for(int j=0; j<16; j++){
          cmds[numCmds]->track[j] = track[j];
        }
        numCmds++;
      }
    }
}

int prevVal = 0;
int volume = 2;
int minVol = 100;
int maxVal = 2;

void incrementVolume(int diff){
  int sign = diff/abs(diff);
  volume -= 2*sign;
  if(volume>minVol) volume=minVol;
  else if(volume<maxVal) volume=maxVal;
  mp3->setVolume(volume, volume);
}

void volumeEncoder(int pin){
  int newVal = analogRead(pin);
  
  int diff = newVal-prevVal;
  
  if(abs(diff)>15&&abs(diff)<300){
    incrementVolume(diff);
    prevVal=newVal;
  }
  else if(abs(diff)>600){
    if(diff<0) diff+=1023;
    else if(diff>0) diff-=1023;
    incrementVolume(diff);
    prevVal=newVal;
  }
}

void volumePot(int pin){
  int newVal = analogRead(pin);
  
  volume = map(newVal,0,1023,100,0);
  
  if(volume>minVol) volume=minVol;
  else if(volume<maxVal) volume=maxVal;
  mp3->setVolume(volume, volume);
}

void audioControl::idle(){
 if(volEnc>=0) volumeEncoder(volEnc);
 if(volPot>=0) volumePot(volPot);
 for(int i=0; i<numCmds; i++){
   cmds[i]->execute();
 }
}
