#! /bin/bash

echo $1
echo $2
echo $3

gst-launch-1.0 -e nvarguscamerasrc sensor-id=$1 ! "video/x-raw(memory:NVMM), width=1920, height=1080, framerate=30/1" ! nvv4l2h264enc ! h264parse ! mpegtsmux ! filesink location=$3 &

PID=$!
sleep $2

kill -INT $PID