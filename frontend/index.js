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

if (process.env.NODE_ENV != 'test') {
	process.stdin.pipe(csvStream);
} else { // shim data
	var inputStream = new stream.PassThrough();
	inputStream.pipe(csvStream);

	function getRandomArbitary (min, max) {
		return Math.random() * (max - min) + min;
	}

	function getRandomInt (min, max) {
		return Math.floor(Math.random() * (max - min + 1)) + min;
	}

	// Format: temperature, tempInDegrees
	setInterval(function () {
		inputStream.write('temperature,' + getRandomArbitary(10, 30) + '\n');
	}, 1000);

	// Format: rssi, nodeId, dbi
	setInterval(function () {
		inputStream.write('rssi,' + getRandomInt(0, 5) + ',' + getRandomArbitary(-100, -30) + '\n');
	}, 1000);
}