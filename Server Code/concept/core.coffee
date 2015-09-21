# early concept for cpre492

app = require('express')()
http = require('http').Server(app)
udoo = require('udoo')

app.use(require('express').static(__dirname + '/public'))

http.listen 3000, ->
  console.log('listening on *:3000')

# set pins
zone1 = udoo.outputPin(23)
zone2 = udoo.outputPin(25)
zone3 = udoo.outputPin(27)
zone4 = udoo.outputPin(29)
zone5 = udoo.outputPin(31)
zone6 = udoo.outputPin(33)
zone7 = udoo.outputPin(35)

zone1.setHigh()