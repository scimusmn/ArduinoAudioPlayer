#include "audioControl.h"
#include "Arduino.h"
#include <Adafruit_VS1053.h>
//#include <SdFat.h>
//#include <ArduinoStream.h>

//ArduinoOutStream cout(Serial);

static bool audioInterrupt = false;
static bool once = false;
static bool hold = false;
static bool playing = false;
static bool invertPS = false;
static bool stopOnRelease = false;
static int debounceTime =0;

Adafruit_VS1053_FilePlayer * mp3 = 0;

audioCmd::audioCmd(){
  input=0;
  lTimer =0;
  numTracks = 0;
  invert = false;
  pressed =false;
  curTrack = 0;
  //memset(track,0,16);
}

void audioCmd::execute(){
  if(type==LOOP){
    if(mp3->stopped()) play(curTrack);
  }
  else if(type==BUTTON_PRESS){
    bool bRead = digitalRead(input);
    if(!invert) bRead = !bRead;
    
    if(!bRead){
      pressed = false;
      if(stopOnRelease) mp3->stopPlaying();
    }
    else if(!pressed&&bRead){
      lTimer = debounceTime+millis();
      elapsed = false;
      pressed=true;
      Serial.println("Pressed!");
    }
    
    if(lTimer<millis()&&!elapsed&& pressed&&!once) lTimer = debounceTime+millis(), elapsed = false;
    
    if(debounce()){
      if(audioInterrupt||(!audioInterrupt&&mp3->stopped())){
        if(bRead&&numTracks){
          mp3->stopPlaying();
          //Serial.print ("Playing ");
          //Serial.print(track[numTracks-1]);
          play(0);
          playing=true;
        }
      }
    }
  } else if(type == BUTTON_SELECT){
    bool bRead = digitalRead(input);
    //if(!invert) bRead = !bRead;
    
    if(!pressed&&bRead){
      Serial.println(bRead);
      lTimer = debounceTime+millis();
      elapsed = false;
      pressed=true;
      Serial.println("Pressed!");
    } else if(!bRead){
      pressed = false;
    }
    
    if(lTimer<millis()&&!elapsed&& pressed&&!once){
      lTimer = debounceTime+millis();
      elapsed = false;
    }
    
    if(debounce()){
      if(bRead&&numTracks){
        mp3->stopPlaying();
        Serial.println("Change track");
        curTrack = (curTrack +1)%numTracks;
        play(curTrack);
        playing=true;
      }
    }
    
    if(mp3->stopped()) play(curTrack);
  }
  else if(type==POT_SELECT){
    int newVal = analogRead(input);
  
    int newTrack = map(newVal,0,1023,0,numTracks);
    if(invertPS){
      if(newTrack!=0) newTrack = numTracks-newTrack;
    }
    int segment = 1023/numTracks;
    
    if(newTrack!=nextTrack&&(newVal%segment)>segment/4&&(newVal%segment)<3*segment/4){
      //Serial.print("New track key is ");
      //Serial.println(abs((nextTrack-newTrack)%numTracks),DEC);
      //if(abs(nextTrack-newTrack)==1||abs(nextTrack-newTrack)==numTracks-1){
        Serial.print("New track is ");
        Serial.println(newTrack,DEC);
        nextTrack=newTrack;
        mp3->stopPlaying();
        mp3->softReset();
        lTimer = debounceTime+millis();
        elapsed = false;
      //}
    }
    
    if(debounce()){
      if(audioInterrupt||(!audioInterrupt&&mp3->stopped())){
        if(nextTrack!=curTrack){
          if(nextTrack<numTracks) curTrack=nextTrack;
          else curTrack=numTracks-1;
          play(curTrack);
        }
      } 
    }
  }
}

void audioCmd::play(int which){
  char trk[track[which].length()+2];
  track[which].toCharArray(trk,track[which].length()+2);
  Serial.print ("Playing ");
  Serial.println(which);
  mp3->startPlayingFile(trk);
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

void audioControl::setup(Adafruit_VS1053_FilePlayer * dMP3){
    mp3 = dMP3;
    File cfg = SD.open("config.txt");
    int line_number = 0;
    int linePos = 0;
  
    while (cfg.peek()>=0){//sdin.getline(buffer, 32, '\n') || sdin.gcount()) {
      char newRead=cfg.read();
      if(newRead>=32) buffer[linePos++] = newRead;              //if newRead is a printed char
      if(linePos>=32||cfg.peek()=='\n'||cfg.peek()=='\r'||cfg.peek()<=0){
        String curLine="";
        curLine=buffer;
        Serial.print(line_number,DEC);
        Serial.print(": ");
        Serial.println(buffer);
        linePos=0;
        line_number++;
        if(curLine.indexOf("volEncoder")>=0) volEnc = curLine.substring(curLine.indexOf('=')+1).toInt();
        else if(curLine.indexOf("volPot")>=0) volPot = curLine.substring(curLine.indexOf('=')+1).toInt();
        else if(curLine.indexOf("interrupt")>=0) audioInterrupt = curLine.substring(curLine.indexOf('=')+1).toInt();
        else if(curLine.indexOf("debounce")>=0) debounceTime = curLine.substring(curLine.indexOf('=')+1).toInt();
        else if(curLine.indexOf("invertPotSelect")>=0) invertPS = curLine.substring(curLine.indexOf('=')+1).toInt();
        else if(curLine.indexOf("once")>=0) once = curLine.substring(curLine.indexOf('=')+1).toInt(),Serial.println("once!");//cout << "once" << endl;
        else if(curLine.indexOf("stopOnRelease")>=0) stopOnRelease = curLine.substring(curLine.indexOf('=')+1).toInt();
        else if(curLine.indexOf("volume")>=0){
          int newVol = map(curLine.substring(curLine.indexOf('=')+1).toInt(),0,100,127,2);
          mp3->setVolume(newVol,newVol);
        }
        else if(curLine.indexOf("loop")>=0){
          String track = curLine.substring(curLine.indexOf('=')+1);
          Serial.println(track);
          cmds[numCmds] = new audioCmd();
          cmds[numCmds]->type=LOOP;
          cmds[numCmds]->track[0]=track;
          cmds[numCmds]->numTracks=1;
          numCmds++;
        }
        else if(curLine.indexOf("pushButton")>=0){
          int val = curLine.substring(curLine.indexOf('[')+1,curLine.indexOf(']')).toInt();
          String track = curLine.substring(curLine.indexOf('=')+1);
          cmds[numCmds] = new audioCmd();
          cmds[numCmds]->type=BUTTON_PRESS;
          cmds[numCmds]->track[0]=track;
          Serial.println(track);
          if(curLine.indexOf("!")>=0) cmds[numCmds]->invert=true;//cout << "Invert" << endl;
          cmds[numCmds]->input=val+14;
          Serial.println(cmds[numCmds]->track[0]);
          Serial.print("Button on pin ");
          Serial.println(cmds[numCmds]->input,DEC);
          pinMode(cmds[numCmds]->input,INPUT_PULLUP);
          cmds[numCmds]->numTracks=1;
          numCmds++;
        }
        else if(curLine.indexOf("buttonSelect")>=0){
          int which = curLine.substring(curLine.indexOf('[')+1,curLine.indexOf(']')).toInt();
          signed int found = -1;
          for(signed int i=0; i<numCmds; i++){
            if(which+14==cmds[i]->input) found=i;
          }
          if(found<0){
            cmds[numCmds] = new audioCmd();
            found = numCmds;
            cmds[numCmds]->type=BUTTON_SELECT;
            cmds[numCmds]->input=which+14;
            pinMode(cmds[numCmds]->input,INPUT);
            Serial.print("ButtonSelect on pin ");
            Serial.println(cmds[numCmds]->input,DEC);
            numCmds++;
          }
          int trackNum = curLine.substring(curLine.indexOf('(')+1,curLine.indexOf(')')).toInt();
          String track = curLine.substring(curLine.indexOf('=')+1);
          if(cmds[found]->track[trackNum].length()<1) cmds[found]->numTracks++;
          cmds[found]->track[trackNum] = track;
          Serial.print("New track: ");
          Serial.print(cmds[found]->track[trackNum]+" with number ");
          Serial.println(cmds[found]->numTracks,DEC);
        }
        else if(curLine.indexOf("potSelect")>=0){
          int which = curLine.substring(curLine.indexOf('[')+1,curLine.indexOf(']')).toInt();
          signed int found = -1;
          for(signed int i=0; i<numCmds; i++){
            if(which==cmds[i]->input) found=i;
          }
          if(found<0){
            cmds[numCmds] = new audioCmd();
            found = numCmds;
            cmds[numCmds]->type=POT_SELECT;
            cmds[numCmds]->input=curLine.substring(curLine.indexOf('[')+1,curLine.indexOf(']')).toInt();
            Serial.print("PotSelect on pin ");
            Serial.println(cmds[numCmds]->input,DEC);
            numCmds++;
          }
          int trackNum = curLine.substring(curLine.indexOf('(')+1,curLine.indexOf(')')).toInt();
          String track = curLine.substring(curLine.indexOf('=')+1);
          if(cmds[found]->track[trackNum].length()<1) cmds[found]->numTracks++;
          cmds[found]->track[trackNum] = track;
          Serial.print("New track: ");
          Serial.print(cmds[found]->track[trackNum]+" with number ");
          Serial.println(cmds[found]->numTracks,DEC);
        }
        memset(buffer,0,sizeof(buffer));
        curLine="";
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

void audioControl::trigger(){
 for(int i=0; i<numCmds; i++){
   cmds[i]->play(0);
 }
}
