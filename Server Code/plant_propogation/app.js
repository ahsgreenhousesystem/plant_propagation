var express = require('express');
var path = require('path');
var logger = require('morgan');
var bodyParser = require('body-parser');
//var udoo = require('udoo');
var schedule = require('node-schedule');

var routes = require('./routes/index');
// var users = require('./routes/users');

var app = express();

// hooking up Mongo DB
var mongo = require('mongodb');
var monk = require('monk');
var db = monk('localhost:27017/plant_propogation');

app.use(express.static(__dirname + '/public'));

// view engine setup   
app.set('views', path.join(__dirname, 'views'));   
app.set('view engine', 'jade');    
    
app.use(logger('dev'));    
app.use(bodyParser.json());    
app.use(bodyParser.urlencoded({ extended: false }));

// Make our db accessible to our route
app.use(function (req, res, next) {
  req.db = db;
  next();
});

app.use('/', routes);

/*
 * Theoretical UDOO Code
 *
 
// set pins
var zone1 = udoo.outputPin(23);
var zone2 = udoo.outputPin(25);
var zone3 = udoo.outputPin(27);
var zone4 = udoo.outputPin(29);
var zone5 = udoo.outputPin(31);
var zone6 = udoo.outputPin(33);
var zone7 = udoo.outputPin(35);
var timeDelay = 1000;
*/

// dummy values
var zone1 = 23;
var timeDelay = 1000;

// turn on sprinkler
function sprinklerOn(zone) {
  //setTimeout(function(){ zone.setHigh(); }, timeDelay);
  setTimeout(function(){ console.log(zone + " ON!"); }, timeDelay);
}

// turn off sprinkler
function sprinklerOff(zone) {
  //setTimeout(function(){ zone.setLow(); }, timeDelay);
  setTimeout(function(){ console.log(zone + " OFF!"); }, timeDelay);
}


/*
 * Theoretical Scheduling Code
 * https://github.com/tejasmanohar/node-schedule/wiki/Date-based-Scheduling
 *
*/

// load jobs
// TODO: loop through all dates from database
//       schedule jobs for each date (if isn't already scheduled)
function loadJob() {
  // loop through db and populate these values for each entry
  var scheduledDate = new Date("October 30, 2015 21:09:00");
  var scheduledJob  = "sprinklerOn";
  var scheduledZone = zone1; 

  // scheduling the job (cancel using j.cancel())
  if (scheduledJob === "sprinklerOn") 
    var j = schedule.scheduleJob(scheduledDate, function(){ sprinklerOn(scheduledZone); });
  else if (scheduledJob === "sprinklerOff")
    var j = schedule.scheduleJob(scheduledDate, function(){ sprinklerOff(scheduledZone); });
  else
    console.log("The scheduledJob variable is not valid.");
}
loadJob();

// add job
// TODO: add job to database
//       loadJobs()
function addJob(date, job, zone) {
}

// remove job
// TODO: remove job from database
//       loadJobs() 
function removeJob(id) {
}

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
