var arduino = require('duino');

var board = new arduino.Board({
  debug: true,
  baudrate: 19200
});

var zone1 = new arduino.Led({
  board: board,
  pin: 53
});

board.on('ready', function(){
  sprinklerOn(zone1);

  setTimeout(function(){
    sprinklerOff(zone1);
  }, 10000);
});

var timeDelay = 700;

function sprinklerOn(zone) {
  setTimeout(function(){
    console.log("Zone 1 on!");
    zone.on(); //this will be opposite...relay acts on low
  }, timeDelay);
}

function sprinklerOff(zone) {
  setTimeout(function(){
    console.log("Zone 1 off!");
    zone.off(); //this will be opposite...relay acts on low
  }, timeDelay);
}
