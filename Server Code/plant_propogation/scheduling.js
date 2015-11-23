var udoo = require('udoo');
var schedule = require('node-schedule');

// hooking up Mongo DB
var mongo = require('mongodb');
var monk = require('monk');
var db = monk('localhost:27017/plant_propogation'); 

//WHEN SCHEDULING, NEED TO CHECK IF ZONE IS ON AUTO *****

/*
 * Theoretical UDOO Code
 */

// zone number, pin, state
//pull/store these with the db? 
var zonePins = {
  '1' : udoo.outputPin(23), 
  '2' : udoo.outputPin(25),
  '3' : udoo.outputPin(27),
  '4' : udoo.outputPin(29), 
  '5' : udoo.outputPin(31), 
  '6' : udoo.outputPin(33), 
  '7' : udoo.outputPin(35)
};

var timeDelay = 700;

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
  setTimeout(function(){ 
    console.log("Zone " + zone + " ON!"); 
    //could add a callback. 
    zonePins[zone].setLow();
  }, timeDelay);
}

// turn off sprinkler
function sprinklerOff(zone) {
  setTimeout(function(){ 
    console.log("Zone " + zone[0] + " OFF!"); 
    //could add a callback
    zonePins[zone].setHigh();
  }, timeDelay);
}


/*
 * Theoretical Scheduling Code
 * https://github.com/tejasmanohar/node-schedule/wiki/Date-based-Scheduling
 *
*/

// load every job (run every time a job is added/deleted)
var jobs = [];
function loadJobs() {

  // cancel any previous jobs
  cancelJobs();

  var zones = db.get('zones').find();

  // loop through each zone & add jobs for watering
  for (i = 0; i < zones.length; i++) { 
    var scheduledZone = zones.zone;
    var times = zones[i].times;

    for (j = 0; j < times.length; j++) {

      var begin = times[j].begin;
      var end = times[j].end;
  
      var scheduledBeginTime = new schedule.RecurrenceRule();
      scheduledBeginTime.hour = getHour(begin);
      scheduledBeginTime.minute = getMinute(begin);
      scheduledBeginTime.second = getSecond(begin);

      var scheduledEndTime = new schedule.RecurrenceRule();
      scheduledEndTime.hour = getHour(end);
      scheduledEndTime.minute = getMinute(end);
      scheduledEndTime.second = getSecond(end);
   
      jobs.push(schedule.scheduleJob(scheduledBeginTime, function(){ sprinklerOn(scheduledZone); }));
      jobs.push(schedule.scheduleJob(scheduledEndTime, function(){ sprinklerOff(scheduledZone); }));
    }
  }
}

// cancel every job 
function cancelJobs() {
  while(jobs.length > 0) {
    jobs.pop().cancel();
  }

  var zones = db.get('zones').find();

  // make sure sprinklers are off
  setTimeout(function() {
    for (i = 0; i < zones.length; ++i) {
      sprinklerOff(zones[i].zone);
    }
  }, timeDelay*2);
}

function getHour(time) {
  //return hour of given time (0-23)
  console.log(time);
  return '';
}

function getMinute(time) {
  //return minute of given time
  return '';
}

function getSecond(time) {
  //return second of given time
  return '';
}

loadJobs();
// cancelJobs();
console.log("There are " + jobs.length + " jobs scheduled.");

