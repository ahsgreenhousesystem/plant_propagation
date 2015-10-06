var express = require('express');
var router = express.Router();
var path = require('path');

/* GET home page. */
router.get('/', function(req, res) {
    //res.render('index.html');
    res.sendFile(path.join(__dirname+'/public/index.html'));
});


/* GET json values for all zones */
router.get('/allZones', function(req, res) {
    // var db = req.db;
    // var collection = db.get('zonecollection');
    // collection.find({},{},function(e,docs){
    //     res.send({
    //         "zones": docs
    //     });
    // });
	res.send({"zone" : 1});
});


/* GET overview page. */
router.get('/overview', function(req, res) {
    res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) +'/public/index.html'));
});

/* GET config page. */
router.get('/config', function(req, res) {
    res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) +'/public/config.html'));
});

/* GET logs page. */
router.get('/logs', function(req, res) {
    res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) +'/public/logs.html'));
});

/* GET users page. */
router.get('/users', function(req, res) {
    res.sendFile(path.join(__dirname.substring(0, __dirname.length - 7) +'/public/users.html'));
});

/* POST to Config */
router.post('/config', function(request, result) {
    var db = request.db;

    var zone = req.body.zone;

    var collection = db.get('zonecollection');

    collection.update({
        "zone" : zone,
        "times" : req.body.times
    }, function (err, doc) {
        if (err) {
            result.send("There was an issue adding the information to the database.");
        } else {
            result.send("Times successfully updated.");
        }
    })
});

/* POST to Setup */
router.post('/setup', function(request, result) {
    var db = request.db;
    var zone = req.body.zone;

    var collection = db.get('zonecollection');

    collection.update({
        "zone" : zone, 
        "name" : req.body.name,
        "active" : req.body.active

    }, function (err, doc) {
        if (err) {
            result.send("There was an issue adding the information to the database.");
        } else {
            result.send("Zone successfully updated."); 
        }
    })
});

module.exports = router;
