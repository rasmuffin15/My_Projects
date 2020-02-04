function confirmPSWD() {

	var password = document.getElementById("password");
	var cpassword = document.getElementById("Cpassword");
	var button = document.getElementById("submit");

	if (password.value === "") {
		return;
	}
	console.log("Password: " + password.value + ", Confirm Password" + cpassword.value);

	if (password.value === cpassword.value) {
		button.disabled = false;
	} else if (password.value === "") {
		button.disabled = true;
	} else {
		button.disabled = true;
	}
}