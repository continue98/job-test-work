
var express = require('express');
var app = express();
var path = require('path');
const bodyParser = require("body-parser");

try {
  addon = require(path.join(__dirname +  '/node_addon/build/Release/get-user-windows.node'));
} catch (err) {
  addon = require(path.join(__dirname +  '/node_addon/build/Debug/get-user-windows.node'));
}


// viewed at http://localhost:8080

app.use(bodyParser.urlencoded({
    extended: true
}));
app.use(bodyParser.json());

app.get('/', function(req, res) {
    res.sendFile(path.join(__dirname + '/html/form.html'));
});
app.post('/params', function(req, res) {
	var username = req.body.username;
	if(username == "")
	{
		res.send("username is clear!");
		res.status(200);
		return;
	}
	var user_exists = (addon.isExistsUserWindows(username) == 1) ? "существует" :  "не существует";
	res.send("Пользователь " + username + " " + user_exists);
	res.status(200);
});
app.listen(8080);

