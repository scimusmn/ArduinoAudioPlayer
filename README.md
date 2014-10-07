##Readme file for ArduinoAudioPlayer##

This project's goal is to provide an Arduino powered media player, which is configured through a simple config file. The workings of this config file will be explained below. This file will also cover the basic installation process for new Audio players.

The instructions below should work to configure both the Adafruit and Sparkfun flavors of the media player.

*1. Install the Arduino IDE:* Download the latest version of the Arduino IDE from http://arduino.cc/en/main/software and move the program to your Application or Program Files folder. Once there, start and quit the program. This will create a folder called "Arduino" in your "Documents" folder.

*2. Copy the "libraries" folder from the repository to your "Documents/Arduino" folder:* This folder contains the 
    libraries necessary to compile the firmware.

*3. Copy the "AudioPlayer_Adafruit" and "AudioPlayer_Sparkfun" folders to the "Documents/Arduino" folder:* These folders contain the firmware which reads and executes the config file.

*4. Determine which "flavor" of player you are using:* If you are using the red Sparkfun mp3 player shield, open the "AudioPlayer_Sparkfun.ino" in the "AudioPlayer_Sparkfun" folder. Else, if you are using the blue Adafruit "Music Maker" shield, open the "AudioPlayer_Adafruit.ino" file in the "AudioPlayer_Adafruit" folder.

*5. Plug the Arduino into your computer using a USB-A to USB-B cable.*

*6. With the file open in the Arduino IDE, select "Arduino Uno" from the "Tools->Boards" menu:* Also, in the "Tools->Serial Port" menu, select the items that starts "/dev/tty.usbmodem...", if you are using OSX. If you are using Windows, note which com ports are listed in the menu, unplug the arduino, and see which entry disappears. Re-plug the arduino and select the port which reappears.

*7. Press the "Upload" button:* It should be in the upper left hand corner of the window, and should look like an arrow pointing right. The report in the teal bar at the bottom of the window should say "Compiling...", then "Uploading...", and finally "Done Uploading." If you receive an error, make sure that the board is connected, that "Arduino Uno" is selected under "Tools->Boards", and that the correct Serial Port is connected.

*8. Disconnect the Arduino*

*9. Compose the "config.txt" file which gives instructions to the media player:* The firmware will respond to the following commands in the "config.txt" file. Each command is written in the form KEYWORD=value, and commands are case sensitive. See the "Keyword" section below for information about the commands available.

*10. Load the media files and "config.txt" file onto the SD card:* Insert the SD card into your computer and load the files. Eject the card and click it onto the arduino shield.


#Keywords

**KEYWORD: interrupt**

*ACCEPTED VALUES:* [0,1]

*RESULT:* If value is "0", any track which is currently playing will play in its entirety before another track may be played. Else, if value is "1", currently playing tracks may be stopped to play another track.

**KEYWORD: debounce**

*ACCEPTED VALUES:* [any integer from 0 to 65536]

*RESULT:* Tell the program how long to wait (in milliseconds) after a button press before another button press can be detected. This prevents bad contacts in switches from double triggering an action.

**KEYWORD: once**

*ACCEPTED VALUES:* [0,1]

*RESULT:* If value is "1", the audio player will only play the given audio clip once per button press. If "0", the track will play repeatedly, until the button is released.

**KEYWORD: stopOnRelease**

*ACCEPTED VALUES:* [0,1]

*RESULT:* If "1", a track that was triggered by a button press will stop when the button is released. Else, the audio file will continue to play on release.

**KEYWORD: loop**

*ACCEPTED VALUES:* any audio file track name as it appears on the memory card.

*RESULT:* If this command appears, the given audio file will loop continuously (unless the interrupt keyword is set, in which case the looping audio can be interrupted by a track from a button press.) Multiple tracks may be queued to loop in sequence using this command; simply add more "loop" keywords to queue more tracks.

**KEYWORD: pushButton[BUTTON_NUMBER]**

*ACCEPTED VALUES:* any audio file track name as it appears on the memory card.

*EXTRA OPTIONS:* Preceed this command with a "!" to invert it's operation

*EXAMPLES:* 

pushButton[0]=track001.mp3   -> plays track001.mp3 when pressed.

!pushButton[0]=track001.mp3   -> plays track001.mp3 when released.

*RESULT:* Tells the audioPLayer to trigger a certain track from input from a button attached to A0-A5 on the arduino.

**KEYWORD: volEncoder**

*ACCEPTED VALUES:* an integer from 0 to 5, corresponding to a the analog input to which the potentiometer is connected.

*RESULT:* Tells the arduino to listen on an analog input pin, dictated by the value, to increment or decrement the volume. If the knob is turned clockwise, it will increment the volume upward, and if turned CCW, it will decrement the volume.

**KEYWORD: volPot**

*ACCEPTED VALUES:* an integer from 0 to 5, corresponding to a the analog input to which the potentiometer is connected.

*RESULT:* Tells the arduino to listen on an analog input pin, dictated by the value, to directly set the volume. Fully CW on the pot is max volume, and fully CCW is minimum.

**KEYWORD: volume**

*ACCEPTED VALUES:* an integer from 0 to 100, corresponding to a volume level 0-100%

*RESULT:* Changes the default volume at which the shield produces audio.

**KEYWORD: potSelect**

*ACCEPTED VALUES:* any audio file track name as it appears on the memory card.

*EXAMPLES:* 

potSelect[0](0)=track001.mp3       -> using input 0, add track001.mp3 as the first track to select
potSelect[0](1)=track002.mp3       -> using input 0, add track002.mp3 as the second track to select
potSelect[0](2)=track003.mp3       -> using input 0, add track003.mp3 as the third track to select

Using the above code, the player would automatically divide the pot into 3 divisions, and play one of each of the list tracksper division.

*RESULT:* Tells the arduino to listen on an analog input pin, dictated by the value, to select between the tracks listed in the configuration file.

