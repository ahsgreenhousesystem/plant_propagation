$(document).ready(function() {

    /*
    Load information into page. 
         
    This isn't going to be here.  On load of page, need to supply html pages with data to fill in 
    all sections of the page. This may have to be done when the page is sent from the server. 
         
    You guys have any suggestions on best way to do this?  Probably done in index.js, but not sure how.  
    */

    $(".timeTable").each(function() {
        var noScheduledTimesDiv = $(this).closest(".panel-body").find(".noScheduledTimes");
        var rows = $(this).find("tr").length;
        if (rows == 1) {
            noScheduledTimesDiv.show();
            $(this).hide();
        }
    });

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
                'zone': zone,
                'times[]': [{
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
            }, 'json');
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

    $(".addTime").bind("click", function(){
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        $("#beginTime").val("");
        $("#endTime").val("");
        $("#zoneToAddTime").val(zoneNumber);
        $("#newTimeModal").modal("show");
    });

    $("#addTimeBtn").bind("click", function() {
        addTimeToZone();
    });

    $(".deleteZone").bind("click", function() {
        var me = $(this);
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        var options = {
            message: "Are you sure you want to delete Zone " + zoneNumber + "?",
            title: 'Delete confirmation'
         };
        eModal.confirm(options).then(function (/* DOM */) { me.closest(".row").remove(); });
    });

    $(document).on("click", ".deleteTime", function() {
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        var noScheduledTimesDiv = $(this).closest(".panel-body").find(".noScheduledTimes");
        $(this).closest("tr").remove();
        var rows = $("#timeTable" + zoneNumber + " tr").length;
        if (rows == 1) {
            noScheduledTimesDiv.show();
            $("#timeTable" + zoneNumber).hide();
        }
    });

    function addTimeToZone() {
        var zoneNumber = $("#zoneToAddTime").val();
        var table = $("#timeTable" + zoneNumber + " tbody");
        var beginTime = $("#beginTime").val();
        var endTime = $("#endTime").val();
        var html = '<tr>';
        html += '<td style="width:250px;">';
        html += '<button class="btn btn-sm btn-success deleteTime"><span class="glyphicon glyphicon-remove"></span>&nbsp;Delete Time</button>';
        html += '</td>';
        html += '<td class="beginTimeTd">';
        html += beginTime;
        html += '</td>';
        html += '<td class="endTimeTd">';
        html += endTime;
        html += '</td>';
        html += '</tr>';
        table.append(html);
        table.closest("div").find(".noScheduledTimes").hide();
        $("#timeTable" + zoneNumber).show();
    }
});
