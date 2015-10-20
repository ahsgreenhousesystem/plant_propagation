var express = require('express');
var path = require('path');
var logger = require('morgan');
// udoo = require('udoo');

var routes = require('./routes/index');
// var users = require('./routes/users');

var app = express();

// hooking up Mongo DB
var mongo = require('mongodb');
var monk = require('monk');
var db = monk('localhost:27017/plant_propogation');

app.use(express.static(__dirname + '/public'));

// Make our db accessible to our route
app.use(function (req, res, next) {
  req.db = db;
  next();
});

app.use('/', routes);

/*
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
*/

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});

module.exports = app;
