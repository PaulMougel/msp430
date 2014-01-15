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
		io.sockets.emit('temperature', parseInt(row[1]));
	}
	return null;
});

if (process.env.NODE_ENV != 'test') {
	process.stdin.pipe(csvStream);
} else { // shim data
	var inputStream = new stream.PassThrough();
	inputStream.pipe(csvStream);
	setInterval(function () {
		inputStream.write('temperature,' + Math.random() * 30 + '\n'); // randrom temp. up to 30deg.
	}, 1000);
}