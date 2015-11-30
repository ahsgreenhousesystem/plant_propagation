var udoo = require('udoo');
var schedule = require('node-schedule');

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
	}, 

  loadJobs : function (zones) {
    loadJobs(zones);
  }, 
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
function loadJobs(zones) {

  // cancel any previous jobs
  cancelJobs(zones);


  // loop through each zone & add jobs for watering
  console.log("zones: " + zones.length);
  for (i = 0; i < zones.length; i++) { 
    
    if(zones[i].active !== 'true') {
      console.log("continueing on zone " + zones[i].zone);
      continue; 
    }

    var scheduledZone = zones.zone;
    var times = zones[i].times;

    console.log(times.length + " for zone " + zones[i].zone);

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

      console.log("pushed job for zone " + zones[i].zone);
   
      jobs.push(schedule.scheduleJob(scheduledBeginTime, function(){ sprinklerOn(scheduledZone); }));
      jobs.push(schedule.scheduleJob(scheduledEndTime, function(){ sprinklerOff(scheduledZone); }));

    }
  }

  console.log("There are " + jobs.length + " jobs scheduled."); 
}

// cancel every job 
function cancelJobs(zones) {
  while(jobs.length > 0) {
    jobs.pop().cancel();
  }

  // make sure sprinklers are off
  setTimeout(function() {
    for (i = 0; i < zones.length; ++i) {
      sprinklerOff(zones[i].zone);
    }
  }, timeDelay*2);
}

function getHour(time) {
  var splits = time.split(':');
  console.log(splits[0]);
  return  parseInt(splits[0]);
}

function getMinute(time) {
  var splits = time.split(':');
    console.log(splits[1]);
  return  parseInt(splits[1]);
}

function getSecond(time) {
  var splits = time.split(':');
  console.log(splits[2]);
  return  parseInt(splits[2]);
}


