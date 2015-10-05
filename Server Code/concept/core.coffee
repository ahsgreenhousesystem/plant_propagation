# early concept for cpre492
app = require('express')()
express = require('express')
http = require('http').Server(app)
path = require('path')
# udoo = require('udoo')

app.use(express.static(__dirname + '/public'))

routes = require('./routes/index')
#users = require('./routes/users')

#hooking up Mongo DB
mongo = require('mongodb')
monk = require('monk')
db = monk('localhost:27017/plant_propogation')

http.listen 3000, ->
  console.log('listening on *:3000')

app.post('/', (request, response) ->
  console.log("Zone: " + request.query.zone + " Action: " + request.query.action)
  response.end()
)

# Make our db accessible to our route
app.use((req, res, next)-> 
	req.db = db
	next
	)
app.use('/', routes)

###
# set pins
zone1 = udoo.outputPin(23)
zone2 = udoo.outputPin(25)
zone3 = udoo.outputPin(27)
zone4 = udoo.outputPin(29)
zone5 = udoo.outputPin(31)
zone6 = udoo.outputPin(33)
zone7 = udoo.outputPin(35)
timeDelay = 1000

# turn on sprinkler
sprinklerOn = (zone) ->
  setTimeout( ->
    zone.setHigh()
  timeDelay)

# turn off sprinkler
sprinklerOff = (zone) ->
  setTimeout( ->
    zone.setLow()
  timeDelay)
###
