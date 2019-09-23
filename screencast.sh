#!/usr/bin/env bash

x=$1
y=$2
width=$3
height=$4
host=$5
port=$6
ffmpeg -f x11grab -video_size 1600x900 -r 10 -i :0.0 -f rawvideo -s "$width"x"$height" -vcodec rawvideo -pix_fmt rgb24 pipe:1 2>/dev/null | ./raw2pixflood $x $y $width $height | nc $host $port
