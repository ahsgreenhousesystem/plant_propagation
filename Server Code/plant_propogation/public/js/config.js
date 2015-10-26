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

    $(".addTime").bind("click", function(){
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        $("#beginTime").val("");
        $("#endTime").val("");
        $("#zoneToAddTime").val(zoneNumber);
		clearValidation();
        $("#newTimeModal").modal("show");
    });
	
	function clearValidation() {
		$("#modal-error-message").empty();
		$("#modal-error-message").hide();
	}

    $("#addTimeBtn").bind("click", function() {
		var beginTime = $("#beginTime");
        var endTime = $("#endTime");
		
		var beginTimeArr = beginTime.val().split(":");
		var beginTimeArr2 = beginTimeArr[1].split(" ");
		var beginHour = beginTimeArr[0];
		var beginMinute = beginTimeArr2[0];
		var beginAmFm = beginTimeArr2[1];
		var endTimeArr = endTime.val().split(":");
		var endTimeArr2 = endTimeArr[1].split(" ");
		var endHour = endTimeArr[0];
		var endMinute = endTimeArr2[0];
		var endAmFm = endTimeArr2[1];
		
		var startTimeObject = new Date();
		startTimeObject.setHours(beginHour, beginMinute, "00");
		
		var endTimeObject = new Date();
		endTimeObject.setHours(endHour, endMinute, "00");
		
		if(startTimeObject > endTimeObject || startTimeObject.getTime() === endTimeObject.getTime()) {
			var errorMessage = "You cannot have a start time later than the end time."
			$("#modal-error-message").empty();
			$("#modal-error-message").append(errorMessage);
			$("#modal-error-message").show();
		} else if(beginTime.val() == "") {
			var errorMessage = "Please enter a start time."
			$("#modal-error-message").empty();
			$("#modal-error-message").append(errorMessage);
			$("#modal-error-message").show();
		} else if(endTime.val() == "") {
			var errorMessage = "Please enter an end time."
			$("#modal-error-message").empty();
			$("#modal-error-message").append(errorMessage);
			$("#modal-error-message").show();
		} else {
			$("#newTimeModal").modal("hide");
			addTimeToZone();
		}
    });

    $(".deleteZone").bind("click", function() {
        var me = $(this);
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        var options = setModalConfirmationOptions("Are you sure you want to delete Zone " + zoneNumber + "?", "Delete Confirmation");
        eModal.confirm(options).then(function (/* DOM */) {
			me.closest(".row").remove(); 
			//remove zone
			 $.post("/removeZone", {
				"zoneNumber": zoneNumber
			}, function(response) {
				alert(response);
				console.warn(response);
				console.warn('zone remove');
			}, 'json');
		});
    });

    $(document).on("click", ".deleteTime", function() {
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        var me = $(this);
        var options = setModalConfirmationOptions("Are you sure you want to delete this time?", "Delete Confirmation");
        eModal.confirm(options).then(function (/* DOM */) {
            var noScheduledTimesDiv = me.closest(".panel-body").find(".noScheduledTimes");
            me.closest("tr").remove();
            var rows = $("#timeTable" + zoneNumber + " tr").length;
            if (rows == 1) {
                noScheduledTimesDiv.show();
                $("#timeTable" + zoneNumber).hide();
            }
        });
        updateTimes(zoneNumber);
    });

    function setModalConfirmationOptions(message, title) {
        var options = {
            message: message,
            title: title
         };
         return options;
    }

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

function updateTimes(zone) {

        var timeArr = [];
        var count = 0;

        $('#timeTable' + zone + ' > tbody > tr').each(function() {
            var begin = $(this).find('.beginTimeTd').text();
            var end = $(this).find('.endTimeTd').text();

            timeArr.push({'begin': begin, 'end' : end});
            count++;
        });

        var data = { 'zone': zone, 'count': count, 'times': timeArr};

        $.post("/config", data, 
        function(response) {
                $("#zone" + zone + "response").text(response);
            }, 'json');
};
