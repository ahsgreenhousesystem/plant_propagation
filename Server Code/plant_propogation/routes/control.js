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
	var collection = req.db.get('zoneStatus');
	
	console.log("Action: " + action);
	
	var zoneNum = zone.charAt(4);
	console.log("Zone: " + zoneNum);
	

	var toggle = true;

	switch(action) {
		case ACTIONS.OPEN: 
			scheduler.sprinklerOn(zoneNum);
			console.log("Zone " + zoneNum + " opened.");
			res.send("Zone opened.");
			break;
		case ACTIONS.CLOSE:
			scheduler.sprinklerOff(zoneNum);
			console.log("Zone " + zoneNum + " closed.");
			res.send("Zone closed.");
			break;
		default: 
			//nothing as of yet
	}

});

module.exports = router;
