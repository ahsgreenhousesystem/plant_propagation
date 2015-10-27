var express = require('express');
var router = express.Router();
var path = require('path');

var scheduler = require('../scheduling');

var ACTIONS = {
	OPEN : "Open", 
	CLOSE : "Close", 
	AUTO : "Auto"
};

router.post('', function(req, res) {
	var zone = req.body.zone;
	var action = req.body.action;
	var collection = req.db.get('zoneStatus');

	var toggle = true;

	switch(action) {
		case ACTIONS.OPEN: 
			//scheduler.sprinklerOn(zone);
			res.send("Zone opened.");
			break;
		case ACTIONS.CLOSE:
			//scheduler.sprinklerOff(zone);
			res.send("Zone closed.");
			break;
		case ACTIONS.AUTO:
			// collection.update({"zone":zone}, 
			// {"zone": zone, "auto":true}, {upsert: true}, function (err, doc) {});
			toggle = false;
			res.send("Zone set to auto.");
			break;
		default: 
			//nothing as of yet
	}

	if(toggle) {
		collection.update({"zone":zone}, 
			{"zone": zone, "auto":false}, 
			{upsert: true}, function (err, doc) {});
	}

});

module.exports = router;