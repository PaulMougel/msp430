var stream = require('stream');

function getRandomArbitary (min, max) {
	return Math.random() * (max - min) + min;
}

function getRandomInt (min, max) {
	return Math.floor(Math.random() * (max - min + 1)) + min;
}

var inputStream = new stream.PassThrough();

inputStream.write('node_id,temperature,rssi,help\n');

inputStream.generateData = function () {
	var randomTemp = getRandomArbitary(10, 30);
	var randomNodeID = getRandomInt(0, 5);
	var randomRSSI = getRandomInt(-100, 0);
	var randomHelp = getRandomInt(0, 10) === 0 ? 1 : 0;
	this.write(randomNodeID + ',' + randomTemp + ',' + randomRSSI + ',' + randomHelp + '\n');
}

module.exports = inputStream;
