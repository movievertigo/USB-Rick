# LilyGO T-Dongle S3 video player
## Introduction
* Plays pre-scaled mjpeg files from the SD card.
* Pressing the button changes to the next video.
* Long pressing the button rotates the playback by 180 degrees.
* Both the current video selection and current rotation are saved in the flash ram, so that they can be restored when you plug the T-Dongle back in.
## Instructions
* Modify the convert.bat file to crop and scale the videos of your choice, but you must make sure that they end up at 160x80 resolution.
* The frame rate will default to 30FPS, but adding -fpsXX to the end of the filename will override this. e.g. NeverGonna-fps25.mjpg
* Copy the converted files to the root of a MicroSD card and insert into the T-Dongle
* Plug in the T-Dongle and the first video will play and loop
* Pressing the button will cycle through the videos, and long pressing will rotate by 180 degrees.
* If the frame rate drops below the requested FPS, the LED will flash red.  If this happens, try reducing the quality setting of the conversion or try a faster MicroSD card
