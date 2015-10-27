$(document).ready(function() {
    $.get("/allLogs", function(response) {
        var zoneLogs = '';
        var zones = [];
        var userTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
        userTable += '<th style="text-align:center">Log Type</th>';
        userTable += '<th style="text-align:center">Date</th>';
        userTable += '<th style="text-align:center">Information</th>';
        userTable += '</thead><tbody>';
        for (var i = 0; i < response.length; i++) {
            if (response[i].type.indexOf("User") > -1) {
                userTable += '<tr><td>' + response[i].type + '</td><td>' + response[i].date + '</td><td>' + response[i].info + '</td></tr>';
            } else {
                if (zones.indexOf(response[i].info.charAt(4)) < 0) {
                    zones.push(response[i].info.charAt(4));
                }
            }
        }
        for (var i = 0; i < zones.length; i++) {
            var zone = zones[i]; // Zone logs must start with 'Zone#'  
            var zonePanel = '<div class="panel panel-default" id="panel' + zone + '"><div class="panel-heading"><h4 class="panel-title">';
            zonePanel += '<a data-toggle="collapse" data-target="#collapse' + zone + '" href="#collapse' + zone + '" class="collapsed">';
            zonePanel += 'Zone ' + zone + '</a></h4></div>';
            zonePanel += '<div id="collapse' + zone + '" class="panel-collapse collapse"><div class="panel-body">';
            var zoneTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
            zoneTable += '<th style="text-align:center">Log Type</th>';
            zoneTable += '<th style="text-align:center">Date</th>';
            zoneTable += '<th style="text-align:center">Information</th>';
            zoneTable += '</thead><tbody>';
            for (var j = 0; j < response.length; j++) {
                if (response[j].info.charAt(4) == zone) {
                    zoneTable += '<tr><td>' + response[j].type + '</td><td>' + response[j].date + '</td><td>' + response[j].info + '</td></tr>';
                }
            }
            zoneTable += '</tbody></table>';
            zonePanel += zoneTable;
            zonePanel += '</div></div></div>';
            zoneLogs += zonePanel;
        }
        userTable += '</tbody></table>';
        $('#userLogs').html(userTable);
        $('#zoneLogs').html('<br>' + zoneLogs);
    });
});