$(document).ready(function() {
    $.get("/allLogs", function(response) {
        for (var i = 0; i < response.length; i++) {
           // fill logs
        }
    });
});