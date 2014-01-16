var stream = require('stream');

function getRandomArbitary (min, max) {
	return Math.random() * (max - min) + min;
}

function getRandomInt (min, max) {
	return Math.floor(Math.random() * (max - min + 1)) + min;
}

var inputStream = new stream.PassThrough();
inputStream.generateData = function () {
	// Format: temperature, tempInDegrees
	this.write('temperature,' + getRandomArbitary(10, 30) + '\n');
	// Format: rssi, nodeId, dbi
	this.write('rssi,' + getRandomInt(0, 5) + ',' + getRandomArbitary(-100, -30) + '\n');
}

module.exports = inputStream;