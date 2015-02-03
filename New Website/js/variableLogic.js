cmd = "";
var Zone1_state = 0;
var Zone2_state = 0;
var Zone3_state = 0;
var Zone4_state = 0;
var Zone5_state = 0;
var Zone6_state = 0;
var Zone7_state = 0;
var Zone8_state = 0;
var Zone9_state = 0;
var Zone10_state = 0;
var Zone11_state = 0;
var Zone12_state = 0;
var Zone13_state = 0;
var Zone14_state = 0;
var Zone15_state = 0;
var Zone16_state = 0;

var loadData = 1;

function getZoneStates() {
	var request = new XMLHttpRequest();
	request.onreadystatechange = function() {
		if (this.readyState == 4) {
			if (this.status == 200) {
				if ((this.responseXML != null) && (loadData == 1)) {
					// XML file received - contains analog values, switch values
					// and LED states
					var count;
					// XML file received - contains analog values, switch values
					// and LED states
					var count = 0;
					// get config parameters
					var num_an = this.responseXML.getElementsByTagName('name').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("name")[count].innerHTML = this.responseXML
								.getElementsByTagName('name')[count].childNodes[0].nodeValue;
						document.getElementsByClassName("name1")[count].innerHTML = this.responseXML
								.getElementsByTagName('name')[count].childNodes[0].nodeValue;
						document.getElementsByClassName("name2")[count].value = this.responseXML
								.getElementsByTagName('name')[count].childNodes[0].nodeValue;
						document.getElementsByClassName("updateTimes")[count].innerHTML = "Update"
								+ this.responseXML.getElementsByTagName('name')[count].childNodes[0].nodeValue
								+ "Times";
					}
					var num_an = this.responseXML
							.getElementsByTagName('visible').length;
					for (count = 0; count < num_an; count++) {
						if (this.responseXML.getElementsByTagName('visible')[count].childNodes[0].nodeValue == "1") {
							document.getElementsByClassName("zone")[count].style.display = "block"
							document.getElementsByClassName("config")[count].style.display = "block"
							document.getElementsByClassName("visible")[count].checked = true

						} else {
							document.getElementsByClassName("zone")[count].style.display = "none"
							document.getElementsByClassName("config")[count].style.display = "none"
							document.getElementsByClassName("visible")[count].checked = false
						}
					}
					var num_an = this.responseXML.getElementsByTagName('log').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("log")[count].value = this.responseXML
								.getElementsByTagName('log')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML
							.getElementsByTagName('logLine').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("logLine")[count].value = this.responseXML
								.getElementsByTagName('logLine')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML.getElementsByTagName('time1').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("time1")[count].value = this.responseXML
								.getElementsByTagName('time1')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML
							.getElementsByTagName('duration1').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("duration1")[count].value = this.responseXML
								.getElementsByTagName('duration1')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML.getElementsByTagName('time2').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("time2")[count].value = this.responseXML
								.getElementsByTagName('time2')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML
							.getElementsByTagName('duration2').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("duration2")[count].value = this.responseXML
								.getElementsByTagName('duration2')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML.getElementsByTagName('time3').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("time3")[count].value = this.responseXML
								.getElementsByTagName('time3')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML
							.getElementsByTagName('duration3').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("duration3")[count].value = this.responseXML
								.getElementsByTagName('duration3')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML.getElementsByTagName('time4').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("time4")[count].value = this.responseXML
								.getElementsByTagName('time4')[count].childNodes[0].nodeValue;
					}
					var num_an = this.responseXML
							.getElementsByTagName('duration4').length;
					for (count = 0; count < num_an; count++) {
						document.getElementsByClassName("duration4")[count].value = this.responseXML
								.getElementsByTagName('duration4')[count].childNodes[0].nodeValue;
						if ((count >= 15) && (loadData == 1)) {
							document.getElementById("dvLoading").style.display = "none"
							document.getElementById("dvOverview").style.display = "block"
							loadData = 0;
						}

					}

				}
			}
		}
	}

	request.open("GET", "ajax_inputs" + cmd, true);
	request.send(null);
	setTimeout('GetZoneStates()', 10000);
	cmd = "";
}

function overviewClick(button) {
	if (button.length == 3) {
		var command = button.substring(0, 1);
		var zone = button.substring(1, 3);
	} else {
		var command = button.substring(0, 1);
		var zone = button.substring(1, 2);
	}
	cmd = "&button," + zone + "," + command;
	document.getElementById("test").innerHTML = cmd;
}

function configClick(zone) {
	cmd = "&config";
	cmd = cmd.concat("," + zone);
	zone = zone - 1;
	cmd = cmd.concat("," + document.getElementsByClassName("time1")[zone].value
			+ "," + document.getElementsByClassName("duration1")[zone].value);
	cmd = cmd.concat("," + document.getElementsByClassName("time2")[zone].value
			+ "," + document.getElementsByClassName("duration2")[zone].value);
	cmd = cmd.concat("," + document.getElementsByClassName("time3")[zone].value
			+ "," + document.getElementsByClassName("duration3")[zone].value);
	document.getElementById("test").innerHTML = cmd;
}

function setupClick(zone) {
	var active = 0;
	cmd = "&setup"
	cmd = cmd.concat("," + zone);
	zone = zone - 1;
	if (document.getElementsByClassName("visible")[zone].checked)
		active = 1;
	else
		active = 0;

	cmd = cmd.concat("," + document.getElementsByClassName("name2")[zone].value
			+ "," + active);
	document.getElementById("test").innerHTML = cmd;
}