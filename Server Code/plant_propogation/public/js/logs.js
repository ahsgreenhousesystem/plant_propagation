$(document).ready(function() {
    $.get("/allLogs", function(response) {
        var zoneLogs = '';
        var zones = [];
        var contactTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
        contactTable += '<th>Log Type</th>';
        contactTable += '<th>Date</th>';
        contactTable += '<th>Information</th>';
        contactTable += '</thead><tbody>';
        for (var i = 0; i < response.length; i++) {
            var type = response[i].type;
            if(type != null) {

                if (type.indexOf("Contact") > -1) {
                    contactTable += '<tr><td style="text-align:left">' + type + '</td>';
                    contactTable += '<td style="text-align:left">' + response[i].date + '</td>';
                    contactTable += '<td style="text-align:left">' + response[i].info + '</td></tr>';
                } else {
                    
                    if (type.length >= 5 && zones.indexOf(type.charAt(4)) < 0) {
                        zones.push(type.charAt(4));
                    }
                }
            }
        }
        for (var i = 0; i < zones.length; i++) {
            var zone = zones[i]; // Zone log types must start with 'Zone#'
            var zonePanel = '<div class="panel panel-default" id="panel' + zone + '"><div class="panel-heading"><h4 class="panel-title">';
            zonePanel += '<a data-toggle="collapse" data-target="#collapse' + zone + '" href="#collapse' + zone + '" class="collapsed">';
            zonePanel += 'Zone ' + zone + '</a></h4></div>';
            zonePanel += '<div id="collapse' + zone + '" class="panel-collapse collapse"><div class="panel-body">';
            var zoneTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
            zoneTable += '<th>Log Type</th>';
            zoneTable += '<th>Date</th>';
            zoneTable += '<th>Information</th>';
            zoneTable += '</thead><tbody>';
            for (var j = 0; j < response.length; j++) {
                var type = response[j].type;
                if (type && type.charAt(4) == zone) {
                    zoneTable += '<tr><td style="text-align:left">' + response[j].type + '</td>';
                    zoneTable += '<td style="text-align:left">' + response[j].date + '</td>';
                    zoneTable += '<td style="text-align:left">' + response[j].info + '</td></tr>';
                }
            }
            zoneTable += '</tbody></table>';
            zonePanel += zoneTable;
            zonePanel += '</div></div></div>';
            zoneLogs += zonePanel;
        }
        contactTable += '</tbody></table>';
        $('#contactLogs').html(contactTable);
        $('#zoneLogs').html('<br>' + zoneLogs);
    });
});