var express = require('express');
var mailer = require("nodemailer");
var router = express.Router();
var path = require('path');

var smtpTransport = mailer.createTransport("SMTP",{
	service: "Gmail",
	auth: {
		user: "ahsgreenhousesystem@gmail.com",
		pass: "greenhouse101"
	}
});

/* GET the different pages */
router.get('/', function(req, res) { res.sendFile(path.join(__dirname + '/public/index.html')); });
router.get('/overview', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/index.html')); });
router.get('/config', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/config.html')); });
router.get('/logs', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/logs.html')); });
router.get('/contacts', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/contacts.html')); });

/* Load json values from the database */
router.get('/allZones', function(req, res) { req.db.get('zones').find({}, {}, function(e, docs) { res.send(docs); }); });
router.get('/allContacts', function(req, res) { req.db.get('contacts').find({}, {}, function(e, docs) { res.send(docs); }); });
router.get('/allLogs', function(req, res) { req.db.get('logs').find({}, {}, function(e, docs) { res.send(docs); }); });

/* POST to Config */
router.post('/config', function(req, res) {
    var db = req.db;

    var zone = req.body.zone;
    var size = req.body.count;

    var timesArr = [];
    for (var i = 0; i < size; i++) {
        var begin = req.body['times[' + i + '][begin]'];
        var end = req.body['times[' + i + '][end]'];

        timesArr.push({
            "begin": begin,
            "end": end
        });
    }

    var collection = db.get('zones');

    collection.update({
        "zone": zone
    }, {
        "zone": zone,
		"name": req.body.name,
        "active": req.body.active,
        "times": timesArr
    }, {
        upsert: true
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue adding the information to the database.");
        } else {
			var logs = req.db.get('logs');
        	logs.insert({
                "type": "Zone" + zone,
                "date": getCurrentDate(),
                "info": "Zone " + zone + " was updated. Name: " + req.body.name + " Active: " + req.body.active + " Times: " + JSON.stringify(timesArr)
            });
            res.send("Times successfully updated.");
        }
    });
});

/* POST to Setup */
router.post('/setup', function(request, result) {
    var db = request.db;
    var zone = req.body.zone;
    var collection = db.get('zones');
    collection.update({
        "zone": zone,
        "name": req.body.name,
        "active": req.body.active

    }, function(err, doc) {
        if (err) {
            result.send("There was an issue adding the information to the database.");
        } else {
			var logs = req.db.get('logs');
        	logs.insert({
                "type": "Zone" + zone,
                "date": getCurrentDate(),
                "info": "Zone " + zone + " was updated. Name: " + req.body.name + " Active: " + req.body.active
            });
            result.send("Zone successfully updated.");
        }
    })
});

router.post('/addContact', function(req, res) {
    var contacts = req.db.get('contacts');
    var name = req.body.name;
    var email = req.body.email;
    var phone = req.body.phone;
    contacts.insert({
        "name": name,
        "email": email,
        "phone": phone
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue adding the contact in the database.");
        } else {
            var logs = req.db.get('logs');
            logs.insert({
                "type": "Contact Addition",
                "date": getCurrentDate(),
                "info": "name: " + name + " email: " + email + " phone: " + phone
            });
            res.send("The contact was successfully added!");
        }
    })
});

router.post('/updateContact', function(req, res) {
    var contacts = req.db.get('contacts');
    var contactId = req.body.contactId;
    var name = req.body.name;
    var email = req.body.email;
    var phone = req.body.phone;
    contacts.update({
        "_id": contactId
    }, {
        "name": name,
        "email": email,
        "phone": phone
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue updating the contact's information in the database.");
        } else {
            var logs = req.db.get('logs');
            logs.insert({
                "type": "Contact Update",
                "date": getCurrentDate(),
                "info": "contactId: " + contactId + " name: " + name + " email: " + email + " phone: " + phone
            });
            sendEmail(req, "A contact was updated!", "The contact with contactId " + contactId + " was updated. Here is the current information. Name: " + name + ", Email: " + email + ", Phone: " + phone + ".");
            res.send("The contact was successfully updated!");
        }
    })
});

router.post('/deleteContact', function(req, res) {
    var contacts = req.db.get('contacts');
    var contactId = req.body.contactId;
    contacts.remove({
        "_id": contactId
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue deleting the contact in the database.");
        } else {
            var logs = req.db.get('logs');
            logs.insert({
                "type": "Contact Deletion",
                "date": getCurrentDate(),
                "info": "contactId: " + contactId
            });
            sendEmail(req, "A contact was deleted!", "The contact with contactId " + contactId + " was deleted.");
            res.send("The contact was successfully deleted!");
        }
    })
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

function sendEmail(req, subject, body) {
	console.warn(getContactEmails(req));
	var mailOptions={
		to : getContactEmails(),
		subject : subject,
		text : body
	}
	smtpTransport.sendMail(mailOptions, function(error){
		if(error){
			console.warn(error);
		}
	});
}

function getContactEmails(req) {
	var to;
	req.db.get('contacts').find({}, function(err, result) {
    	if (!err) {
    		for(var i = 0; i < result.length; i++) {
        		to += result[i].email + ", ";
        	}
    	}
	});
	return to;
}

module.exports = router;