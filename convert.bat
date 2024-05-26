ffmpeg -y -i NeverGonna.mp4 -ss 0.12 -filter:v "crop=920:460:180:130, scale=160:80" -c:v mjpeg -q:v 1 data/NeverGonna-fps25.mjpg
ffmpeg -y -i BadApple.mp4 -ss 1.46 -filter:v "crop=480:240:0:60, scale=160:80" -c:v mjpeg -q:v 1 data/BadApple-fps30.mjpg
