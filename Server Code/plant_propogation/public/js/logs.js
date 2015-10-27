$(document).ready(function() {
    $.get("/allLogs", function(response) {
    	var zoneLogs = '';
    	var userTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
           		userTable += '<th style="text-align:center">Log Type</th>';
           		userTable += '<th style="text-align:center">Date</th>';
           		userTable += '<th style="text-align:center">Information</th>';
           		userTable += '</thead><tbody>';
        for (var i = 0; i < response.length; i++) {
           if(response[i].type.indexOf("User") > -1) {
           		userTable += '<tr><td>' + response[i].type + '</td><td>' + response[i].date + '</td><td>' + response[i].info + '</td></tr>';
           } else {
           		var zone = response[i].info.charAt(4); // Zone logs must start with 'Zone#'
           		var zonePanel = '<div class="panel panel-default" id="panel' + zone + '"><div class="panel-heading"><h4 class="panel-title">';
           		zonePanel += '<a data-toggle="collapse" data-target="#collapse' + zone + '" href="#collapse' + zone + '" class="collapsed">';
           		zonePanel += 'Zone ' + zone + '</a></h4></div>';
               	zonePanel += '<div id="collapse' + zone + '" class="panel-collapse collapse"><div class="panel-body">';
               	
               	var zoneTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
           		zoneTable += '<th style="text-align:center">Log Type</th>';
           		zoneTable += '<th style="text-align:center">Date</th>';
           		zoneTable += '<th style="text-align:center">Information</th>';
           		zoneTable += '</thead><tbody>';
           		zoneTable += '<tr><td>' + response[i].type + '</td><td>' + response[i].date + '</td><td>' + response[i].info + '</td></tr>';
           		zoneTable += '</tbody></table>';
               	
               	zonePanel += zoneTable;
               	zonePanel += '</div></div></div>';
           		zoneLogs += zonePanel;
           }
        }
        userTable += '</tbody></table>';
        $('#userLogs').html(userTable);
        $('#zoneLogs').html(zoneLogs);
    });
});