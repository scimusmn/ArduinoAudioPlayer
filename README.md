Readme file for ArduinoAudioPlayer

    This project's goal is to provide an Arduino powered media player, 
which is configured through a simple config file. The workings of
this config file will be explained below. This file will also cover the basic 
installation process for new Audio players.

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

*9. Compose the "config.txt" file which gives instructions to the media player:* The firmware will respond to the following commands in the "config.txt" file. Each command is written in the form KEYWORD=value, and commands are case sensitive.

#KEYWORD: interrupt

    ACCEPTED VALUES: [0,1]

    RESULT: If value is "0", any track which is currently playing will play in its entirety before another track may be played. Else, if value is "1", currently playing tracks may be stopped to play another track.

#KEYWORD: debounce

    ACCEPTED VALUES: [any integer from 0 to 65536]

    RESULT: Tell the program how long to wait (in milliseconds) after a button press before another button press can be detected. This prevents bad contacts in switches from double triggering an action.

#KEYWORD: once

    ACCEPTED VALUES: [0,1]

    RESULT: 
