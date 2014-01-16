var stream = require('stream');
var express = require('express');
var csv = require('csv');

// Webserver
var app = express();
var server = app.listen(process.env.PORT || 8080);
var io = require('socket.io').listen(server);
app.use(express.static(__dirname + '/public'));
app.get('/smoothie.js', function (req, res) {
	res.sendfile('./node_modules/smoothie/smoothie.js');
});

// Data input
var csvStream = csv();
csvStream.transform(function (row) {
	if (row[0] === 'temperature') {
		io.sockets.emit('temperature', parseFloat(row[1]));
	}
	else if (row[0] === 'rssi') {
		io.sockets.emit('rssi', {
			nodeId: parseInt(row[1]),
			rssi: parseFloat(row[2])
		});
	}
	return null;
});

var inputStream;
if (process.env.NODE_ENV != 'test') {
	inputStream = process.stdin;
} else { // shim data
	inputStream = require('./shim');
	setInterval(inputStream.generateData.bind(inputStream), 1000);
}

inputStream.pipe(csvStream)