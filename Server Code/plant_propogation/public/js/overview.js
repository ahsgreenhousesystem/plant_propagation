$(function() {
    $("form input[type=submit]").click(function() {
        $("input[type=submit]", $(this).parents("form")).removeAttr("clicked");
        $(this).attr("clicked", "true");
    });
	
	$.get("/allZones", function(response) {
        for (var zone = 0; zone < response.length; zone++) {
			createZoneControlPanel(response[zone]);
		}
    });
	
	function createZoneControlPanel(zoneObject) {
		var controlPanelHtml = '<div class="col-lg-4 col-md-6">';
        controlPanelHtml += '<div class="panel panel-default">';
        controlPanelHtml += '<div class="panel-heading">';
        controlPanelHtml += '<h3 class="panel-title">'+zoneObject.name+'</h3>';
        controlPanelHtml += '</div>';
        controlPanelHtml += '<div class="panel-body">';
        controlPanelHtml += '<div class="btn-group btn-group-lg" role="group" style="padding-bottom: 15px;">';
        controlPanelHtml += '<input type="submit" name="zone'+zoneObject.zone+'" value="Open" class="btn btn-default btn-sm" />';
        controlPanelHtml += '<input type="submit" name="zone'+zoneObject.zone+'" value="Close" class="btn btn-default btn-sm" />';
        controlPanelHtml += '<input type="submit" name="zone'+zoneObject.zone+'" value="Auto" class="btn btn-default btn-sm" />';
        controlPanelHtml += '</div></div></div></div>';
		$("#zoneControlDiv").append(controlPanelHtml);
	}

    $("form").submit(function(e) {
        e.preventDefault();
        var button = $("input[type=submit][clicked=true]"); // the button that was clicked
        var zone = button.attr("name"); // button zone name
        var action = button.val(); // button value (Open/Close/Auto)
        var data = {"zone": zone, "action": action};

        $.post("/control", data, function(response) { // send AJAX request to the web server
            $("#" + zone + "log").text(response); // add notification to the log in the zone button panel
            var zoneButtons = document.getElementsByName(zone); // array of the buttons for that zone
            for (var i = 0; i < zoneButtons.length; i++) {
                zoneButtons[i].setAttribute("class", "btn btn-default btn-sm"); // reset all buttons to default
            }
            button.addClass("btn-success"); // highlight the button that was clicked
        });
    });
});