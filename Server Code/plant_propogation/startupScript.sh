#!/bin/bash

# start database (path might change a little)
mongod --dbpath '/home/udooer/Documents/code/cpre492/dec15-01/Server Code/plant_propogation/data' &

# change to the right directory (path might change a little)
cd '/home/udooer/Documents/code/cpre492/dec15-01/Server Code/plant_propogation'
npm start
