var express = require('express');
var mailer = require("nodemailer");
var path = require('path');
var logger = require('morgan');
var bodyParser = require('body-parser');

var routes = require('./routes/index');
var control = require('./routes/control');
var scheduling = require('./modules/scheduling');

// hooking up Mongo DB
var mongo = require('mongodb');
var monk = require('monk');
var db = monk('localhost:27017/plant_propogation');



var app = express();
var smtpTransport = mailer.createTransport("SMTP",{
	service: "Gmail",
	auth: {
		user: "ahsgreenhousesystem@gmail.com",
		pass: "greenhouse101"
	}
});

app.use(express.static(__dirname + '/public'));

// view engine setup   
app.set('views', path.join(__dirname, 'views'));   
app.set('view engine', 'jade');    
    
app.use(logger('dev'));    
app.use(bodyParser.json());    
app.use(bodyParser.urlencoded({ extended: false }));

// Make our db accessible to our route
app.use(function (req, res, next) {
  req.db = db;
  next();
});

app.use('/', routes);
app.use('/control', control);


// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});

db.get('zones').find({}, {}, function(e, docs) { 
});


module.exports = app;
