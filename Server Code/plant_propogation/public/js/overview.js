$(function() {
    $("form input[type=submit]").click(function() {
        $("input[type=submit]", $(this).parents("form")).removeAttr("clicked");
        $(this).attr("clicked", "true");
    });

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