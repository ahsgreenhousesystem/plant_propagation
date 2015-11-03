//var udoo = require('udoo');
var schedule = require('node-schedule');

//WHEN SCHEDULING, NEED TO CHECK IF ZONE IS ON AUTO *****

/*
 * Theoretical UDOO Code
 *

// zone number, pin, state
var zones = [[1, udoo.outputPin(23), 0],
             [2, udoo.outputPin(25), 0],
             [3, udoo.outputPin(27), 0],
             [4, udoo.outputPin(29), 0],
             [5, udoo.outputPin(31), 0],
             [6, udoo.outputPin(33), 0],
             [7, udoo.outputPin(35), 0]]
var timeDelay = 700;
udoo.reset();
*/

// dummy values: zone number, pin, state (on/off), beginTime[hr][mn], endTime[hr][mn]
// populate these values via the database
var zones = [[1, 23, 0, [23,41], [23,43]],
             [2, 25, 0, [0,0], [0,2]]];
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
  //setTimeout(function(){ zone[1].setLow(); }, timeDelay);
  setTimeout(function(){ console.log("Zone " + zone[0] + " ON!"); }, timeDelay);
  zone[2] = 1;
}

// turn off sprinkler
function sprinklerOff(zone) {
  //setTimeout(function(){ zone[1].setHigh(); }, timeDelay);
  setTimeout(function(){ console.log("Zone " + zone[0] + " OFF!"); }, timeDelay);
  zone[2] = 0;
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

  // loop through each zone & add jobs for watering
  for (i = 0; i < zones.length; ++i) { 
    var scheduledZone = zones[i];
  
    var scheduledBeginTime = new schedule.RecurrenceRule();
    scheduledBeginTime.hour = scheduledZone[3][0];
    scheduledBeginTime.minute = scheduledZone[3][1]; 

    var scheduledEndTime = new schedule.RecurrenceRule();
    scheduledEndTime.hour = scheduledZone[4][0];
    scheduledEndTime.minute = scheduledZone[4][1];
 
    jobs.push(schedule.scheduleJob(scheduledBeginTime, function(){ sprinklerOn(scheduledZone); }));
    jobs.push(schedule.scheduleJob(scheduledEndTime, function(){ sprinklerOff(scheduledZone); }));
  }
}

// cancel every job 
function cancelJobs() {
  while(jobs.length > 0) {
    jobs.pop().cancel();
  }

  // make sure sprinklers are off
  setTimeout(function() {
    for (i = 0; i < zones.length; ++i) {
      if (zones[i][2] === 1) {
        sprinklerOff(zones[i]);
      }
    }
  }, timeDelay*2);
}

loadJobs();
// cancelJobs();
console.log("There are " + jobs.length + " jobs scheduled.");

