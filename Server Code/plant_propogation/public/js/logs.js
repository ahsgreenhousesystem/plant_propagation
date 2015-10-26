$(document).ready(function() {
    $.get("/allLogs", function(response) {
        for (var i = 0; i < response.length; i++) {
           if(response[i].type.indexOf("User") > -1) {
           		var table = '<table class="table table-striped table-condensed table-hover" style="width:100%"><thead><th>Log Type</th><th>Date</th><th>Information</th><tbody>';
           		table += '<tr><td>' + response[i].type + '</td><td>' + response[i].date + '</td><td>' + response[i].info + '</td></tr>';
           		table += '</tbody></table>';
           		$('#userLogs').html(table);
           } else {
           		// add to zone logs
           		
           		/*
           		<div class="panel panel-default" id="panel1">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseOne"
                        href="#collapseOne" class="collapsed">
                     Zone 1
                     </a>
                  </h4>
               </div>
               <div id="collapseOne" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log...
                  </div>
               </div>
            </div>
            <div class="panel panel-default" id="panel2">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseTwo"
                        href="#collapseTwo" class="collapsed">
                     Zone 2
                     </a>
                  </h4>
               </div>
               <div id="collapseTwo" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log...
                  </div>
               </div>
            </div>
            <div class="panel panel-default" id="panel3">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseThree"
                        href="#collapseThree" class="collapsed">
                     Zone 3
                     </a>
                  </h4>
               </div>
               <div id="collapseThree" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log..
                  </div>
               </div>
            </div>
            <div class="panel panel-default" id="panel4">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseFour"
                        href="#collapseFour" class="collapsed">
                     Zone 4
                     </a>
                  </h4>
               </div>
               <div id="collapseFour" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log..
                  </div>
               </div>
            </div>
            <div class="panel panel-default" id="panel5">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseFive"
                        href="#collapseFive" class="collapsed">
                     Zone 5
                     </a>
                  </h4>
               </div>
               <div id="collapseFive" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log..
                  </div>
               </div>
            </div>
            <div class="panel panel-default" id="panel6">
               <div class="panel-heading">
                  <h4 class="panel-title">
                     <a data-toggle="collapse" data-target="#collapseSix"
                        href="#collapseSix" class="collapsed">
                     Zone 6
                     </a>
                  </h4>
               </div>
               <div id="collapseSix" class="panel-collapse collapse">
                  <div class="panel-body">
                     Log..
                  </div>
               </div>
            </div>
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
    });
});