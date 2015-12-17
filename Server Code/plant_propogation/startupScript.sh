#!/bin/bash

# start database (path might change a little)
# mongod --dbpath '/home/udooer/plant_propagation/Server Code/plant_propogation/data' &

# start camera
fswebcam -d '/dev/video2' -r 1280x720 '/home/udooer/plant_propagation/Server Code/plant_propogation/public/streaming.jpg' -l 3 >/dev/null 2>&1 &

# change to the right directory (path might change a little)
cd '/home/udooer/Documents/plant_propagation/Server Code/plant_propogation'
npm start
