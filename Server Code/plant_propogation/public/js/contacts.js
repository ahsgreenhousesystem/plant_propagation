$(document).ready(function() {
    $.get("/allContacts", function(response) {
        for (var i = 0; i < response.length; i++) {
            $("#contactTable").append(addContact(response[i]._id, response[i].name, response[i].email, response[i].phone));
        }
    });
});

$('#newContactModal').on('hidden.bs.modal', function(e) {
    clearNewContactModalFields();
    clearValidation();
});

function removeContact(btn) {
    var options = {
        message: "Are you sure you want to delete this contact?",
        title: "Delete Confirmation"
    };
    eModal.confirm(options).then(function() {
        $(btn).closest("tr").remove();
        var contactId = $(btn).closest("tr").find(".contactId");
        $.ajax({
            url: '/deleteContact',
            data: {
                'contactId': contactId.val()
            },
            method: "post",
            success: function(response) {
                eModal.alert('The contact was successfully deleted!');
                location.reload();
            },
            error: function(response) {
                eModal.alert('The contact was not deleted!');
            }
        });
    });
}

function updateContact(btn) {
    var contactId = $(btn).closest("tr").find(".contactId");
    var name = $(btn).closest("tr").find(".nameField");
    var email = $(btn).closest("tr").find(".emailField");
    var phone = $(btn).closest("tr").find(".phoneField");
    if (validateFields(name, email, phone)) {
        var data = {
            'contactId': contactId.val(),
            'name': name.val(),
            'email': email.val(),
            'phone': phone.val()
        };
        $.ajax({
            url: '/updateContact',
            data: data,
            method: "post",
            success: function(response) {
                eModal.alert('The contact was successfully updated!');
            },
            error: function(response) {
                eModal.alert('The contact was not updated!');
            }
        });
    }
}

function clearNewContactModalFields() {
    $("#newContactName").val("");
    $("#newContactEmail").val("");
    $("#newContactPhoneNumber").val("");
}

function clearValidation() {
    $("#newContactName").parent().removeClass("has-error");
    $("#newContactEmail").parent().removeClass("has-error");
    $("#newContactPhoneNumber").parent().removeClass("has-error");
    $(".nameField").parent().removeClass("has-error");
    $(".emailField").parent().removeClass("has-error");
    $(".phoneField").parent().removeClass("has-error");
}

function validateFields(name, email, phone) {
    clearValidation();
    var allValid = true;
    if (!isContactNameValid(name)) {
        name.parent().addClass("has-error");
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

$("#saveContact").bind("click", function() {
    var name = $("#newContactName");
    var email = $("#newContactEmail");
    var phone = $("#newContactPhoneNumber");
    if (validateFields(name, email, phone)) {
        $("#newContactModal").modal("hide");
        $.post("/addContact", {
            "name": name.val(),
            "email": email.val(),
            "phone": phone.val()
        }, function(response) {
            alert(response);
        }, 'json');
        location.reload();
    }
});

function isContactNameValid(name) {
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

function addContact(contactId, name, email, phone) {
    var contactHtml = '<tr>';
    contactHtml += '<td class="sr-only"><input type="text" class="contactId" placeholder="Contact Id" value="' + contactId + '" /></td>';
    contactHtml += '<td><input type="text" class="form-control nameField" placeholder="Full Name" value="' + name + '" /></td>';
    contactHtml += '<td><input type="text" class="form-control emailField" placeholder="Email Address" value="' + email + '" /></td>';
    contactHtml += '<td><input type="text" class="form-control phoneField" placeholder="Phone Number" value="' + phone + '" /></td>';
    contactHtml += '<td><button type="button" class="btn btn-primary btn-sm" onclick="updateContact(this)"><span class="glyphicon glyphicon-refresh"></span>&nbsp;<span class="hidden-xs">Update</span></button></td>';
    contactHtml += '<td><button type="button" class="btn btn-success btn-sm" onclick="removeContact(this)"><span class="glyphicon glyphicon-remove"></span>&nbsp;<span class="hidden-xs">Delete</span></button></td>';
    contactHtml += '</tr>';
    return contactHtml;
}