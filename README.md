# LilyGO T-Dongle S3 video player
## Introduction
Plays pre-scaled mjpeg files from the SD card.  Pressing the button changes to the next video.  The current selection is saved in the flash ram, so it will restart the same video when you plug it back in.
## Instructions
* Modify the convert.bat file to crop and scale the videos of your choice, but you must make sure that they end up as 160x80 resolution.
* The frame rate will default to 30FPS, but adding -fpsXX to the end of the filename will override this. e.g. NeverGonna-fps25.mjpg
* Copy the converted files to the root of a MicroSD card and insert into the T-Dongle
* Plug in the T-Dongle and the first video will play and loop
* Pressing the button will cycle through the videos
* If the frame rate drops below the requested FPS, the LED will flash red.  If this happens, try reducing the quality setting of the conversion or try a faster MicroSD card
