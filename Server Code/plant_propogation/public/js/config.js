$(document).ready(function() { 

	function hideTimeTable() {
		$(".timeTable").each(function() {
			var noScheduledTimesDiv = $(this).closest(".panel-body").find(".noScheduledTimes");
			var rows = $(this).find("tr").length;
			if (rows == 1) {
				noScheduledTimesDiv.show();
				$(this).hide();
			}
		});
	}

    $.get("/allZones", function(response) {
        for (var zone = 0; zone < response.length; zone++) {
			createZonePanel(response[zone]);
		}
		hideTimeTable();
    });
	
	$.get("/maxtime", function(response) {
		if(response[0] != null && response[0].maxtime != null) {
			var maxtime = response[0].maxtime;
			var maxtimeId = response[0]._id;
			$("#maxWateringTime").val(maxtime);
			$("#maxtimeId").val(maxtimeId);
		}
    });
	
	function createZonePanel(zoneObject) {
		var zoneHtml = '<div class="row">';
        zoneHtml += '<input class="zoneNumber" type="hidden" value="'+zoneObject.zone+'" />';
        zoneHtml += '<div class="col-lg-12 col-md-12">';
        zoneHtml += '<div class="panel panel-default">';
        zoneHtml += '<div class="panel-heading">';
        zoneHtml += '<h3 class="panel-title">';
		if(zoneObject.active == "true") {
			zoneHtml += zoneObject.name + '- <label>Active:&nbsp;</label><input id="A'+zoneObject.zone+'" type="checkbox" class="visible active-checkbox" checked>';
		} else {
			zoneHtml += zoneObject.name + '- <label>Active:&nbsp;</label><input id="A'+zoneObject.zone+'" type="checkbox" class="visible active-checkbox">';
		}
        zoneHtml += '<span class="pull-right">';
        zoneHtml += '<button class="btn btn-default btn-xs addTime"><span class="glyphicon glyphicon-time"></span>&nbsp;<span class="hidden-xs">Add Time</span></button>';
        zoneHtml += '</span>';
        zoneHtml += '</h3>';
        zoneHtml += '</div>';
        zoneHtml += '<div class="panel-body">';
        zoneHtml += '<table id="timeTable'+zoneObject.zone+'" class="table table-striped table-condensed table-hover timeTable" style="text-align:left;">';
        zoneHtml += '<thead>';
        zoneHtml += '<tr>';
        zoneHtml += '<th></th>';
        zoneHtml += '<th>Begin Time</th>';
        zoneHtml += '<th>End Time</th>';
        zoneHtml += '</tr>';
        zoneHtml += '</thead>';
        zoneHtml += '<tbody></tbody>';
        zoneHtml += '</table>';
        zoneHtml += '<div class="noScheduledTimes"><em>There are no scheduled watering times.</em></div>';
        zoneHtml += '</div>';
        zoneHtml += '</div>';
        zoneHtml += '</div>';
        zoneHtml += '<div class="row" id="zone'+zoneObject.zone+'response"></div>';
        zoneHtml += '</div>';
		$("#zoneDiv").append(zoneHtml);
        addConfiguredTimes(zoneObject);
	}

	$("#beginTime").datetimepicker({
        format: 'HH:mm:ss',
		widgetPositioning: {
			veritcal: 'top'
		}
    });
	
	$("#endTime").datetimepicker({
        format: 'HH:mm:ss',
		widgetPositioning: {
			veritcal: 'top'
		}
    });

	$(document).on("click", ".addTime", function() {
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
		
		if(beginTime.val() == "") {
			var errorMessage = "Please enter a start time."
            displayAddTimeErrorMessage(errorMessage);
		} else if(endTime.val() == "") {
			var errorMessage = "Please enter an end time."
			$("#modal-error-message").empty();
            displayAddTimeErrorMessage(errorMessage);
			return false;
		}
		
		var beginTimeArr = beginTime.val().split(":");
		var beginHour = beginTimeArr[0];
		var beginMinute = beginTimeArr[1];
        var beginSecond = beginTimeArr[2];
		var endTimeArr = endTime.val().split(":");
		var endHour = endTimeArr[0];
		var endMinute = endTimeArr[1];
		var endSecond = endTimeArr[2];
		
		var startTimeObject = new Date();
		startTimeObject.setHours(beginHour, beginMinute, beginSecond);

        var startTimePlusMaxTime = new Date();
		var maxWateringTime = parseFloat($("#maxWateringTime").val());
        startTimePlusMaxTime.setHours(beginHour, beginMinute, beginSecond);
		if(maxWateringTime == null || maxWateringTime == "") {
			maxWateringTime = 30;
		}
        startTimePlusMaxTime.setMinutes(startTimePlusMaxTime.getMinutes() + maxWateringTime);
		
		var endTimeObject = new Date();
		endTimeObject.setHours(endHour, endMinute, endSecond);
		
		if(startTimeObject > endTimeObject || startTimeObject.getTime() === endTimeObject.getTime()) {
			var errorMessage = "You cannot have a start time later than the end time."
            displayAddTimeErrorMessage(errorMessage);
			return false;
		}
        if(endTimeObject > startTimePlusMaxTime) {
            var errorMessage = "The sprinkler cannot run for more than " + maxWateringTime +" minutes at a time."
            displayAddTimeErrorMessage(errorMessage);
            return false;
        }
		$("#newTimeModal").modal("hide");
		addNewTime();
    });

    function displayAddTimeErrorMessage(errorMessage) {
        $("#modal-error-message").empty();
        $("#modal-error-message").append(errorMessage);
        $("#modal-error-message").show();
    }

    $(document).on("click", ".deleteTime", function() {
        var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
        var me = $(this);
        var options = setModalConfirmationOptions("Are you sure you want to delete this time?", "Delete Confirmation");
       //eModal.confirm(options).then(function (/* DOM */) {
            var noScheduledTimesDiv = me.closest(".panel-body").find(".noScheduledTimes");
            me.closest("tr").remove();
            var rows = $("#timeTable" + zoneNumber + " tr").length;
            if (rows == 1) {
                noScheduledTimesDiv.show();
                $("#timeTable" + zoneNumber).hide();
            }
        //});
        updateZone(zoneNumber);
    });
	
	$("#hideZonesCbx").bind("click", function() {
		if($(this).is(":checked")) {
			$("#zoneDiv .row").each(function() {
				var noScheduledTimesDiv = $(this).find(".noScheduledTimes");
				if(noScheduledTimesDiv.is(":visible")) {
					$(this).addClass("hide-zone");
				}
			});
		} else {
			$("#zoneDiv .row").removeClass("hide-zone");
		}
	});

    function setModalConfirmationOptions(message, title) {
        var options = {
            message: message,
            title: title
         };
         return options;
    }

    function addNewTime() {
        var zoneNumber = $("#zoneToAddTime").val();
        var beginTime = $("#beginTime").val();
        var endTime = $("#endTime").val();
        addTableRow(zoneNumber, beginTime, endTime);
        updateZone(zoneNumber);
    }

    function addConfiguredTimes(zoneObject) {
		var times = zoneObject.times;
        if(times == null || (times != '' && times.length == 0))
            return;

        var num = zoneObject.zone;
        var times = zoneObject.times;

        for(var i = 0; i < times.length; i++) {
            addTableRow(num, times[i].begin, times[i].end);
        }
    }

    function addTableRow(zone, begin, end) {
        var table = $("#timeTable" + zone + " tbody");
        var html = '<tr>';
        html += '<td style="width:250px;">';
        html += '<button class="btn btn-sm btn-success deleteTime"><span class="glyphicon glyphicon-remove"></span>&nbsp;Delete Time</button>';
        html += '</td>';
        html += '<td class="beginTimeTd">';
        html += begin;
        html += '</td>';
        html += '<td class="endTimeTd">';
        html += end;
        html += '</td>';
        html += '</tr>';
        table.append(html);
        table.closest("div").find(".noScheduledTimes").hide();
        $("#timeTable" + zone).show();
    }


});

$(document).on("click", ".active-checkbox", function() {
	var zoneNumber = $(this).closest(".row").find(".zoneNumber").val();
	updateZone(zoneNumber);
});

$("#updateMaxTimeBtn").bind("click", function() {
	var maxWateringTime = $("#maxWateringTime").val();
	var maxtimeId = $("#maxtimeId").val();
	if(maxtimeId == null || maxtimeId == "") {
		var data = {'maxtime': maxWateringTime}
		$.ajax({
			url: '/maxtimeInsert',
			data: data,
			method: "post",
			success: function(response){
				eModal.alert('Max watering time successfully set!');
				$("#maxtimeId").val("1");
			},
			error: function(){
				eModal.alert('Max watering time did not set!');
			}
		});
	} else if(maxWateringTime != null && maxWateringTime != "") {
		var data = {'id': 1, 'maxtime': maxWateringTime}
		$.ajax({
			url: '/maxtime',
			data: data,
			method: "post",
			success: function(response){
				eModal.alert('Max watering time successfully set!');
			},
			error: function(){
				eModal.alert('Max watering time did not set!');
			}
		});
	} else {
		alert("Please enter in a max watering time!");
	}
	refreshAllTimes();
});

function refreshAllTimes() {
	var maxTime = $("#maxWateringTime").val();
	$(".timeTable tr").each(function() {
		var beginTime = $(this).find(".beginTimeTd").text();
		var endTime = $(this).find(".endTimeTd").text();
		
		var beginTimeArr = beginTime.split(":");
		var beginHour = beginTimeArr[0];
		var beginMinute = beginTimeArr[1];
        var beginSecond = beginTimeArr[2];
		var endTimeArr = endTime.split(":");
		var endHour = endTimeArr[0];
		var endMinute = endTimeArr[1];
		var endSecond = endTimeArr[2];
		
		var startTimeObject = new Date();
		startTimeObject.setHours(beginHour, beginMinute, beginSecond);

        var startTimePlusMaxTime = new Date();
		var maxWateringTime = parseFloat($("#maxWateringTime").val());
        startTimePlusMaxTime.setHours(beginHour, beginMinute, beginSecond);
		if(maxWateringTime == null || maxWateringTime == "") {
			maxWateringTime = 30;
		}
        startTimePlusMaxTime.setMinutes(startTimePlusMaxTime.getMinutes() + maxWateringTime);
		
		var endTimeObject = new Date();
		endTimeObject.setHours(endHour, endMinute, endSecond);
		
		if(endTimeObject > startTimePlusMaxTime) {
			$(this).remove();
		}
	});
}

function updateZone(zone) {

        var timeArr = [];
        var count = 0;

        $('#timeTable' + zone + ' > tbody > tr').each(function() {
            var begin = $(this).find('.beginTimeTd').text();
            var end = $(this).find('.endTimeTd').text();

            timeArr.push({'begin': begin, 'end' : end});
            count++;
        });

        var active = $('#A'+zone).is(':checked');
        //var name = $('');
        var name = "Zone " + zone;

        var data = { 'zone': zone, 'count': count, 'name' : name, 'active' : active, 'times': timeArr};

		$.ajax({
			url: '/config',
			data: data,
			method: "post",
			success: function(response){
				eModal.alert('Zone successfully updated!');
			},
			error: function(){
				eModal.alert('Zone not updated!');
			}
		});
};
