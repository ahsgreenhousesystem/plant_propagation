$(document).ready(function() {

    /*
    Load information into page. 
         
    This isn't going to be here.  On load of page, need to supply html pages with data to fill in 
    all sections of the page. This may have to be done when the page is sent from the server. 
         
    You guys have any suggestions on best way to do this?  Probably done in index.js, but not sure how.  
    */

    $.get("/allZones", function(response) {
       // alert(response);
        for (var zone = 0; zone < response.length; zone++) {
            var num = zone + 1;
            $("#N"+num).text(zone);
            $("#A"+num).prop("checked", response[zone].active);

            for (var i = 1; i <= 3; i++) {
                $("#Z" + num + "T" + i).text(response[zone].times[i-1].begin);
                $("#Z" + num + "D" + i).text(response[zone].times[i-1].end);
            }
        }

    });

    $('.input-sm').datetimepicker({
        format: 'LT'
    });

    //TODO - change to only buttons for individual zones, add another for 'Update All'
    $("button").click(function() {
        var zone = $(this).attr("id");
        var begin1 = $("#Z" + zone + "T1").val();
        var end1 = $('#Z' + zone + "D1").val();
        var begin2 = $('#Z' + zone + "T2").val();
        var end2 = $('#Z' + zone + "D2").val();
        var begin3 = $('#Z' + zone + "T3").val();
        var end3 = $('#Z' + zone + "D3").val();

        $.post("/config", {
                "zone": zone,
                "times": [{
                    "begin": begin1,
                    "end": end1
                }, {
                    "begin": begin2,
                    "end": end2
                }, {
                    "begin": begin3,
                    "end": end3
                }]
            },
            function(response) {
                $("#zone" + zone + "response").text(response);
            });
    });

    //This is from old zones.html (Setup page)
    function setupClick(zone) {
        if (zone) {
            var name = $("#N" + zone).val();
            var active = $("#A" + zone).prop("checked");

            $.post("/setup", {
                "zone": zone,
                "name": name,
                "active": active
            }, function(response) {

            });
        }
    };
});