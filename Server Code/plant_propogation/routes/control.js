var express = require('express');
var router = express.Router();
var path = require('path');

var scheduler = require('../modules/scheduling');

var ACTIONS = {
	OPEN : "Open", 
	CLOSE : "Close", 
};

router.post('', function(req, res) {
	var zone = req.body.zone;
	var action = req.body.action;
	var collection = req.db.get('zoneStatus');

	var toggle = true;

	switch(action) {
		case ACTIONS.OPEN: 
			scheduler.sprinklerOn(zone);
			console.log("Zone " + zone + " opened.");
			res.send("Zone opened.");
			break;
		case ACTIONS.CLOSE:
			scheduler.sprinklerOff(zone);
			console.log("Zone " + zone + " closed.");
			res.send("Zone closed.");
			break;
		default: 
			//nothing as of yet
	}

});

module.exports = router;
