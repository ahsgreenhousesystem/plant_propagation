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
    var db = req.db;
    var collection = db.get('zonecollection');
    collection.find({},{},function(e,docs){
        res.send(docs);
    });
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

// var getTimes = function (json) {
//     console.log("Getting times from json");
//     var times = "[";
//     for(var i = 0; i < 3; i++) {
//         //console.log(json.times[i].begin);
//         console.log("Time at " + i + " : " + json);
//        // console.log(json.times[i][begin]);
//         var begin = json.times[i].begin;
//         var end = json.times[i].end;
//         times += "{\"begin\": " + begin + " , \"end\" : " + end + " }";
//         if(i < 2) times += ", ";
//     }
//     times += "]";

//     return times;
// };

/* POST to Config */
router.post('/config', function(req, res) {
    var db = req.db;

    var zone = req.body;
    console.log(zone);
    ///var times = getTimes(req.body);

    console.log("Times: " + zone.times);
    var times = "[";
    for(var i = 0; i < 3; i++) {
        //console.log(timeArray[i]);
       // console.log(timeArray[i]);
        // var begin = req.body.times[i].begin;
        // var end = req.body.times[i].end;
        // times += "{\"begin\": " + begin + " , \"end\" : " + end + " }";
        // if(i < 2) times += ", ";
    }
    times += "]";
    console.log(times);
    
    console.log("Zone: " + zone.zone);
    //console.log("Times: " + zone.times);

    var collection = db.get('zonecollection');

    collection.update({
        "zone" : zone.zone,
        "times" : times
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
