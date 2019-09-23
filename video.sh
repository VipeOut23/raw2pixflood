#!/usr/bin/env bash
x=$1
y=$2
width=$3
height=$4
host=$5
port=$6
file=$7
ffmpeg -re -stream_loop -1 -i $file \
	-f rawvideo -pix_fmt rgb24 -s "$width"x$height  pipe:1 2>/dev/null \
	| ./raw2pixflood $x $y $width $height \
	| pv -F '%r [Buffer: %T] Total: %b' \
	| nc $host $port
