$(document).ready(function() {
    $.get("/allLogs", function(response) {
    	var userTable = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead>';
           		userTable += '<th style="text-align:center">Log Type</th>';
           		userTable += '<th style="text-align:center">Date</th>';
           		userTable += '<th style="text-align:center">Information</th>';
           		userTable += '</thead><tbody>';
        for (var i = 0; i < response.length; i++) {
           if(response[i].type.indexOf("User") > -1) {
           		userTable += '<tr><td>' + response[i].type + '</td><td>' + response[i].date + '</td><td>' + response[i].info + '</td></tr>';
           } else {
           		
           		// add to zone logs
           		/*
            <div class="panel panel-default" id="panel7">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseSeven"
                        href="#collapseSeven" class="collapsed">
                     Zone 7
                     </a>
                  </h4>
               </div>
               <div id="collapseSeven" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log..
                  </div>
               </div>
            </div>
           		*/
           }
        }
        userTable += '</tbody></table>';
        $('#userLogs').html(userTable);
    });
});