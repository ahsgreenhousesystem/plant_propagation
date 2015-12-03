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

  cancelJobsForZone: function (zone) {
    cancelJobsForZone(zone);
  },

  addJobsForZone: function (zone, timesArr) {
    addJobsForZone(zone, timesArr);
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
    console.log("Zone " + zone + " OFF!"); 
    //could add a callback
    zonePins[zone].setHigh();
  }, timeDelay);
}


/*
 * Theoretical Scheduling Code
 * https://github.com/tejasmanohar/node-schedule/wiki/Date-based-Scheduling
 *
*/

// load every job
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

    var scheduledZone = zones[i].zone;
    var times = zones[i].times;

    console.log(times.length + " for zone " + zones[i].zone);

    for (j = 0; j < times.length; j++) {

      var begin = times[j].begin;
      var end = times[j].end;
  
      addJobSet(scheduledZone, begin, end);
    }
  }

  console.log("There are " + jobs.length + " jobs scheduled."); 
}

function addJobSet(zoneNum, begin, end) {
  console.log("Adding jobset for " + zoneNum);
  jobs.push({'zone': zoneNum, 'job': createJob(zoneNum, begin, true)});
  jobs.push({'zone': zoneNum, 'job': createJob(zoneNum, end, false)});

  console.log("pushed begin and end jobs for zone " + zoneNum);
}

function addJobsForZone(zone, timesArr) {
  console.log("Total Jobs before: " + jobs.length);
  console.log("Adding jobs for zone " + zone);
  for(var i = 0; i < timesArr.length; i++) {
    console.log("begin time : " + timesArr[i].begin);
    console.log("end time : " + timesArr[i].end);
    addJobSet(zone, timesArr[i].begin, timesArr[i].end);
  }
  console.log(timesArr.length*2 + " jobs added.");
  console.log("Total Jobs after: " + jobs.length);
}

function cancelJobsForZone(zone) {
  console.log("Cancelling jobs for zone " + zone);
  var i;
  for(i = 0; i < jobs.length; i++) {
    console.log("job : " + (i + 1));
    if(jobs[i] != null && jobs[i].zone === zone) {
      console.log("cancelling job " + (i+1) + " for zone " + jobs[i].zone);
      jobs[i].job.cancel();
      jobs[i] = null;
    }
  }

  console.log("Cancelled " + i + " jobs.");


  // make sure sprinkler is off
  setTimeout(function() {
      sprinklerOff(zone);
  }, timeDelay*2);
}

// cancel every job 
function cancelJobs(zones) {
  while(jobs.length > 0) {
    jobs.pop().job.cancel();
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

function createJob(zoneNum, time, turnOn) {
  console.log("Creating a job.");
  var rule = new schedule.RecurrenceRule();
  rule.hour = getHour(time);
  rule.minute = getMinute(time);
  rule.second = getSecond(time);

  var job;

  if(turnOn) {
    job = schedule.scheduleJob(rule, function() { sprinklerOn(zoneNum); });
  }
  else {
    job = schedule.scheduleJob(rule, function() { sprinklerOff(zoneNum); });
  }
  //add log statement

  return job;
}

function cancelJob(job) {
  job.cancel();
  //log??
}


