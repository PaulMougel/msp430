var stream = require('stream');

function getRandomArbitary (min, max) {
	return Math.random() * (max - min) + min;
}

function getRandomInt (min, max) {
	return Math.floor(Math.random() * (max - min + 1)) + min;
}

var inputStream = new stream.PassThrough();
inputStream.generateData = function () {
    var randomTemp = getRandomArbitary(10, 30);
    var randomNodeID = getRandomInt(0, 5);
    var randomRSSI = getRandomInt(-100, 0);
    this.write('node_id,' + randomNodeID + ',temperature,' + randomTemp + ',rssi,' + randomRSSI + '\n');
}

module.exports = inputStream;
