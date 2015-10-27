var express = require('express');
var router = express.Router();
var path = require('path');

/* GET the different pages */
router.get('/', function(req, res) { res.sendFile(path.join(__dirname + '/public/index.html')); });
router.get('/overview', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/index.html')); });
router.get('/config', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/config.html')); });
router.get('/logs', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/logs.html')); });
router.get('/users', function(req, res) { res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) + '/public/users.html')); });

/* Load json values from the database */
router.get('/allZones', function(req, res) { req.db.get('zones').find({}, {}, function(e, docs) { res.send(docs); }); });
router.get('/allUsers', function(req, res) { req.db.get('users').find({}, {}, function(e, docs) { res.send(docs); }); });
router.get('/allLogs', function(req, res) { req.db.get('logs').find({}, {}, function(e, docs) { res.send(docs); }); });

/* POST to Config */
router.post('/config', function(req, res) {
    var db = req.db;

    var zone = req.body.zone;
    var size = req.body.count;

    console.log(req.body);

    console.log("Zone: " + zone);
    console.log("Size: " + size);

    var timesArr = [];
    for (var i = 0; i < size; i++) {
        var begin = req.body['times[' + i + '][begin]'];
        var end = req.body['times[' + i + '][end]'];

        timesArr.push({
            "begin": begin,
            "end": end
        });
    }

    console.log(timesArr);

    var collection = db.get('zones');

    console.log("DB Zone: " + collection.find({
        "zone": zone
    }));

    collection.update({
        "zone": zone
    }, {
        "zone": zone,
        "active": req.body.active,
        "name": req.body.name,
        "times": timesArr
    }, {
        upsert: true
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue adding the information to the database.");
            console.log("There was an issue adding the information to the database.");
        } else {
            res.send("Times successfully updated.");
            console.log("Times successfully updated.");
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
            result.send("Zone successfully updated.");
        }
    })
});

router.post('/addUser', function(req, res) {
    var users = req.db.get('users');
    var name = req.body.name;
    var email = req.body.email;
    var phone = req.body.phone;
    users.insert({
        "name": name,
        "email": email,
        "phone": phone
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue adding the user's information in the database.");
        } else {
            var logs = req.db.get('logs');
            logs.insert({
                "type": "User Added",
                "date": getCurrentDate(),
                "info": "name: " + name + " email: " + email + " phone: " + phone
            });
            res.send("The user was successfully added!");
        }
    })
});

router.post('/updateUser', function(req, res) {
    var users = req.db.get('users');
    var userId = req.body.userId;
    var name = req.body.name;
    var email = req.body.email;
    var phone = req.body.phone;
    users.update({
        "_id": userId
    }, {
        "name": name,
        "email": email,
        "phone": phone
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue updating the user's information in the database.");
        } else {
            var logs = req.db.get('logs');
            logs.insert({
                "type": "User Updated",
                "date": getCurrentDate(),
                "info": "userId: " + userId + " name: " + name + " email: " + email + " phone: " + phone
            });
            res.send("The user was successfully updated!"):
        }
    })
});

router.post('/deleteUser', function(req, res) {
    var users = req.db.get('users');
    var userId = req.body.userId;
    users.remove({
        "_id": userId
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue deleting the user's information in the database.");
        } else {
            var logs = req.db.get('logs');
            logs.insert({
                "type": "User Deleted",
                "date": getCurrentDate(),
                "info": "userId: " + userId
            });
            res.send("The user was successfully deleted!");
        }
    })
});

router.post('/deleteZone', function(req, res) {
    var zones = req.db.get('zones');
    var zoneNumber = req.body.zoneNumber;
    zones.remove({
        "zone": zoneNumber
    }, function(err, doc) {
        if (err) {
            res.send("There was an issue deleting the zones's information in the database.");
        } else {
            logs.insert({
                "type": "Zone Deleted",
                "date": getCurrentDate(),
                "info": "zone: " + zoneNumber
            });
            res.send("The zone was successfully deleted!");
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

module.exports = router;