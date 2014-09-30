/**
 * \file MP3Shield_Audio_Player.ino
 *  This is the basic firmware for for the standard SMM Arduino
 *  audio player. Firmware reads a configuration from a file on the SD card
 *  called "config.txt". Details for this file are in the "readme.txt" file of
 *  the git repo.
 */
#include <SPI.h>
//and the MP3 Shield Library
#include <Adafruit_VS1053.h>
#include <SD.h>


#include "audioControl.h"
audioControl audioCtl;

#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)

// These are the pins used for the music maker shield
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3  


Adafruit_VS1053_FilePlayer musicPlayer =// create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void setup() {

  uint8_t result; //result code

  Serial.begin(9600);
  SD.begin(CARDCS); 
 
  audioCtl.setup(&musicPlayer);

  //Initialize the MP3 Player Shield
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
  musicPlayer.setVolume(2, 2);
}


void loop() {
  audioCtl.idle();
}



