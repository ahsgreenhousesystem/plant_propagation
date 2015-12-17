var express = require('express');
var router = express.Router();
var path = require('path');

var scheduler = require('../modules/scheduling');

var ACTIONS = {
	OPEN : "Open Sprinkler",
	CLOSE : "Close Sprinkler",
};

router.post('', function(req, res) {
	var zone = req.body.zone;
	var action = req.body.action;

	console.log("Action: " + action);

	var zoneNum = zone.charAt(4);
	console.log("Zone: " + zoneNum);


	var toggle = true;
	var logs = req.db.get('logs');

	switch(action) {
		case ACTIONS.OPEN:
			scheduler.sprinklerOn(zoneNum);
			console.log("Zone " + zoneNum + " opened.");
			console.log("Logs: " + logs);
			logs.insert({
				"type": "Zone" + zoneNum,
				"date": getCurrentDate(),
				"info": "Opened sprinkler for zone " + zoneNum + "."
			});
			res.send("Zone opened.");
			break;
		case ACTIONS.CLOSE:
			scheduler.sprinklerOff(zoneNum);
			console.log("Zone " + zoneNum + " closed.");
			console.log("Logs: " + logs);
			logs.insert({
				"type": "Zone" + zoneNum,
				"date": getCurrentDate(),
				"info": "Closed sprinkler for zone " + zoneNum + "."
			});
			res.send("Zone closed.");
			break;
		default:
			//nothing as of yet
	}

});

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

module.exports = router;
