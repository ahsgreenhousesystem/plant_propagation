$(document).ready(function() {
	$.get("/allUsers", function(response) {
    	for (var i = 0; i < response.length; i++) {
    		$("#userTable").append(addUser(response[i].name, response[i].email, response[i].phone));
    	}
    });
});

$('#newUserModal').on('hidden.bs.modal', function(e) {
    clearNewUserModalFields();
    clearValidation();
});

function removeUser(btn) {
    var options = {
        message: "Are you sure you want to delete this user?",
        title: "Delete Confirmation"
     };
    eModal.confirm(options).then(function (/* DOM */) { $(btn).closest("tr").remove(); });
    var email = $(btn).closest("tr").find(".emailField");
         $.post("/deleteUser", {
                "email": email.val()
            }, function(response) {
            	alert(response);
            }, 'json');
}

function updateUser(btn) {
    var name = $(btn).closest("tr").find(".fullNameField");
    var email = $(btn).closest("tr").find(".emailField");
    var phone = $(btn).closest("tr").find(".phoneField");
    if (validateFields(name, email, phone)) {
         $.post("/updateUser", {
                "name": name.val(),
                "email": email.val(),
                "phone": phone.val()
            }, function(response) {
            	alert(response);
            }, 'json');
    }
}

function clearNewUserModalFields() {
    $("#newUserFullName").val("");
    $("#newUserEmail").val("");
    $("#newUserPhoneNumber").val("");
}

function clearValidation() {
    $("#newUserFullName").parent().removeClass("has-error");
    $("#newUserEmail").parent().removeClass("has-error");
    $("#newUserPhoneNumber").parent().removeClass("has-error");
    $(".fullNameField").parent().removeClass("has-error");
    $(".emailField").parent().removeClass("has-error");
    $(".phoneField").parent().removeClass("has-error");
}

function validateFields(fullName, email, phone) {
    clearValidation();
    var allValid = true;
    if (!isUserNameValid(fullName)) {
        fullName.parent().addClass("has-error");
        allValid = false;
    }
    if (!isEmailValid(email)) {
        email.parent().addClass("has-error");
        allValid = false;
    }
    if (!isPhoneValid(phone)) {
        phone.parent().addClass("has-error");
        allValid = false;
    }
    return allValid;
}

$("#saveUser").bind("click", function() {
    var name = $("#newUserFullName");
    var email = $("#newUserEmail");
    var phone = $("#newUserPhoneNumber");
    if (validateFields(name, email, phone)) {
        $("#userTable").append(addUser(name.val(), email.val(), phone.val()));
        $("#newUserModal").modal("hide");
         $.post("/addUser", {
                "name": name.val(),
                "email": email.val(),
                "phone": phone.val()
            }, function(response) {
            	alert(response);
            }, 'json');
    }
});

function isUserNameValid(name) {
    if (name.length < 1) {
        return false;
    }
    return true;
}

function isEmailValid(email) {
    var pattern = /^([a-z\d!#$%&'*+\-\/=?^_`{|}~\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]+(\.[a-z\d!#$%&'*+\-\/=?^_`{|}~\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]+)*|"((([ \t]*\r\n)?[ \t]+)?([\x01-\x08\x0b\x0c\x0e-\x1f\x7f\x21\x23-\x5b\x5d-\x7e\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]|\\[\x01-\x09\x0b\x0c\x0d-\x7f\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]))*(([ \t]*\r\n)?[ \t]+)?")@(([a-z\d\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]|[a-z\d\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF][a-z\d\-._~\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]*[a-z\d\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])\.)+([a-z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]|[a-z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF][a-z\d\-._~\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]*[a-z\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])\.?$/i;
    return pattern.test(email.val());
}

function isPhoneValid(phone) {
    var pattern = /\(?([0-9]{3})\)?([ .-]?)([0-9]{3})\2([0-9]{4})/
    return pattern.test(phone.val());
}

function addUser(name, email, phone) {
    var userHtml = '<tr>';
    userHtml += '<td><input type="text" class="form-control fullNameField" placeholder="Full Name" value="' + name + '" /></td>';
    userHtml += '<td><input type="text" class="form-control emailField" placeholder="Email Address" value="' + email + '" /></td>';
    userHtml += '<td><input type="text" class="form-control phoneField" placeholder="Phone Number" value="' + phone + '" /></td>';
    userHtml += '<td><button type="button" class="btn btn-primary btn-sm" onclick="updateUser(this)"><span class="glyphicon glyphicon-refresh"></span>&nbsp;<span class="hidden-xs">Update</span></button></td>';
    userHtml += '<td><button type="button" class="btn btn-success btn-sm" onclick="removeUser(this)"><span class="glyphicon glyphicon-remove"></span>&nbsp;<span class="hidden-xs">Delete</span></button></td>';
    userHtml += '</tr>';
    return userHtml;
}