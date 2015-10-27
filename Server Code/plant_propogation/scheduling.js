//var udoo = require('udoo');
var schedule = require('node-schedule');

//WHEN SCHEDULING, NEED TO CHECK IF ZONE IS ON AUTO ***** 

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

module.exports = {
	sprinklerOn : function (zone) {
		sprinklerOn(zone);
	}, 

	sprinklerOff : function (zone) {
		sprinklerOff(zone);
	}
};


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