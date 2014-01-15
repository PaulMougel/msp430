var express = require('express');
var csv = require('csv');

// Webserver
var app = express();
var server = app.listen(process.env.PORT || 8080);
var io = require('socket.io').listen(server);
app.use(express.static(__dirname + '/public'));

// Data input
var csvStream = csv();
csvStream.transform(function (row) {
	if (row[0] === 'temperature') {
		io.sockets.emit('temperature', parseInt(row[1]));
	}
	return null;
});

process.stdin.pipe(csvStream);