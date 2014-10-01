/**
 * \file MP3Shield_Audio_Player.ino
 *  This is the basic firmware for for the standard SMM Arduino
 *  audio player. Firmware reads a configuration from a file on the SD card
 *  called "config.txt". Details for this file are in the "readme.txt" file of
 *  the git repo.
 */

#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

/**
 * Create the SdFat library.
 */
SdFat sd;

/**
 *  Include the library to read the configuration from the SD card
 *
 */
#include "audioControl.h"
audioControl audioCtl;


SFEMP3Shield MP3player;

void setup() {
  Serial.begin(9600);

  Serial.print(F("Free RAM = ")); // available in Version 1.0 F() bases the string to into Flash, to use less SRAM.
  Serial.print(FreeRam(), DEC);  // FreeRam() is provided by SdFatUtil.h
  Serial.println(F(" Should be a base line of 1040, on ATmega328 when using INTx"));


  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");
  
  audioCtl.setup(&MP3player);

  //Initialize the MP3 Player Shield
  int result = MP3player.begin();
  //check result, see readme for error codes.
  if(result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
    if( result == 6 ) {
      Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
      Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
    }
  }

  //help();
  
  MP3player.setVolume(2, 2);
}


void loop() {
  audioCtl.idle();
}



