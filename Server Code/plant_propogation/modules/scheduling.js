var arduino = require('duino');
var schedule = require('node-schedule');
var mailer = require("nodemailer");
var smtpTransport = mailer.createTransport("SMTP",{
	service: "Gmail",
	auth: {
		user: "ahsgreenhousesystem@gmail.com",
		pass: "greenhouse101"
	}
});

var board = new arduino.Board({
  debug: true,
  baudrate: 19200
});

var zone1 = new arduino.Led({
  board: board,
  pin: 23
});
var zone2 = new arduino.Led({
  board: board,
  pin: 27
});
var zone3 = new arduino.Led({
  board: board,
  pin: 31
});
var zone4 = new arduino.Led({
  board: board,
  pin: 35
});
var zone5 = new arduino.Led({
  board: board,
  pin: 53
});
var zone6 = new arduino.Led({
  board: board,
  pin: 49
});
var zone7 = new arduino.Led({
  board: board,
  pin: 41
});

var zonePins = {
  '1' : zone1,
  '2' : zone2,
  '3' : zone3,
  '4' : zone4,
  '5' : zone5,
  '6' : zone6,
  '7' : zone7
};

var timeDelay = 300;

module.exports = {
	sprinklerOn : function (zone) {
		sprinklerOn(zone);
	},

	sprinklerOff : function (zone) {
		sprinklerOff(zone);
	},

  loadJobs : function (zones, db) {
    loadJobs(zones, db);
  },

  cancelJobsForZone: function (db, zone) {
    cancelJobsForZone(db, zone);
  },

  addJobsForZone: function (db, zone, timesArr) {
    addJobsForZone(db, zone, timesArr);
  },

};


// turn on sprinkler
function sprinklerOn(zone) {    
  setTimeout(function() {
      console.log("Zone " + zone + " ON!");
      zonePins[zone].off();
    }, timeDelay);
}

// turn off sprinkler
function sprinklerOff(zone) {
    setTimeout(function() {
      console.log("Zone " + zone + " OFF!");
      zonePins[zone].on();
    }, timeDelay);
}


/*
 * Theoretical Scheduling Code
 * https://github.com/tejasmanohar/node-schedule/wiki/Date-based-Scheduling
 *
*/

// load every job
var jobs = [];
function loadJobs(zones, db) {

  if(!zones) {
    console.log("No zones to load jobs for.");
    return;
  }

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

      addJobSet(db, scheduledZone, begin, end);
    }
  }

  console.log("There are " + jobs.length + " jobs scheduled.");
}

function addJobSet(db, zoneNum, begin, end) {
  console.log("Adding jobset for " + zoneNum);
  jobs.push({'zone': zoneNum, 'job': createJob(db, zoneNum, begin, true)});
  jobs.push({'zone': zoneNum, 'job': createJob(db, zoneNum, end, false)});

  console.log("pushed begin and end jobs for zone " + zoneNum);
}

function addJobsForZone(db, zone, timesArr) {
  console.log("Total Jobs before: " + jobs.length);
  console.log("Adding jobs for zone " + zone);
  for(var i = 0; i < timesArr.length; i++) {
    console.log("begin time : " + timesArr[i].begin);
    console.log("end time : " + timesArr[i].end);
    addJobSet(db, zone, timesArr[i].begin, timesArr[i].end);
  }
  console.log(timesArr.length*2 + " jobs added.");
  console.log("Total Jobs after: " + jobs.length);

  var zoneJobs = [];
  for(var i = 0; i < jobs.length; i++) {
    if(jobs[i] != null && jobs[i].zone == zone) {
      zoneJobs.push(jobs[i]);
    }
  }

  var logs = db.get('logs');
  console.log("Logs are null ? : " + logs == null);
  logs.insert({
    "type": "Zone" + zone,
    "date": getCurrentDate(),
    "info": "Added jobs to zone " + zone + ".  Jobs: " + JSON.stringify(zoneJobs)
  });
}

function cancelJobsForZone(db, zone) {
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

  console.log("Logging jobs.");
  var logs = db.get('logs');
  console.log('Got logs.' + logs);
  console.log("Logs are null ? : " + logs == null);
  logs.insert({
    "type": "Zone" + zone,
    "date": getCurrentDate(),
    "info": "Cancelled jobs to zone " + zone + "."
  });


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

function createJob(db, zoneNum, time, turnOn) {
  console.log("Creating a job.");
  var rule = new schedule.RecurrenceRule();
  rule.hour = getHour(time);
  rule.minute = getMinute(time);
  rule.second = getSecond(time);

  var job;
  var logs = db.get('logs');

  if(turnOn) {
    job = schedule.scheduleJob(rule, function() {
       sprinklerOn(zoneNum);
       logs.insert({
         "type": "Zone" + zoneNum,
         "date": getCurrentDate(),
         "info": "Open sprinkler JOB fired for zone " + zoneNum + "."
       });
    sendEmail(db, "Zone " + zoneNum + " was turned ON automatically!", "This is just a notification to let you know that zone " + zoneNum + " was turned ON automatically.");
     });
  }
  else {
    job = schedule.scheduleJob(rule, function() {
      sprinklerOff(zoneNum);
      logs.insert({
        "type": "Zone" + zoneNum,
        "date": getCurrentDate(),
        "info": "Close sprinkler JOB fired for zone " + zoneNum + "."
      });
    sendEmail(db, "Zone " + zoneNum + " was turned OFF automatically!", "This is just a notification to let you know that zone " + zoneNum + " was turned OFF automatically.");
    });
  }
  return job;
}

function sendEmail(db, subject, body) {
	db.get('contacts').find({}, function(err, result) {
    	if (!err) {
    		for(var i = 0; i < result.length; i++) {
    			var mailOptions={
					to : result[i].email,
					subject : subject,
					text : body
				}
				smtpTransport.sendMail(mailOptions, function(error){
					if(error){
						console.warn(error);
					}
				});
        	}
    	}
	});
}

function cancelJob(job) {
  job.cancel();
}

function getCurrentDate() {
    var currentdate = new Date();
    var datetime = (currentdate.getMonth() + 1) + "/" + currentdate.getDate() + "/" + currentdate.getFullYear() + " @ " + leftPad(currentdate.getHours(), 2) + ":" + leftPad(currentdate.getMinutes(), 2) + ":" + leftPad(currentdate.getSeconds(), 2);
    return datetime;
}

function leftPad(num, size) {
    var s = num + "";
    while (s.length < size) s = "0" + s;
    return s;
}
