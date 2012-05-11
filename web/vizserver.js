/**
 Web server that serves up visualization code for quack catcher network

 1) Install NodeJS: http://nodejs.org/#download
 2) npm install socket.io
 3) npm install serialport
 4) run from cmd line: 'node vizserver.js'

 **/

var webport = 8124;
var dataport = 8125;
var hostname = process.argv[2] || '127.0.0.1';
var serialPath = "/dev/tty.SLAB_USBtoUART";
var samplesPerPage = 50; // How many x,y,z accelerometer readings do we get per second?

var http = require('http');
var net = require('net');
var path = require('path');
var fs = require('fs');

/** 
 Create Web Server
 **/

var contentTypes = { '.js'	: 'text/javascript'
	, '.html'	: 'text/html'
	, '.htm'	: 'text/html'
	, '.css'	: 'text/css'
	, '.jpg'	: 'image/jpeg'
	, '.jpeg'	: 'image/jpeg'
	, '.gif'	: 'image/gif'
	, '.png'	: 'image/png'
}

function getContentType(path) {
	var extname = path.extname(path);
}

function handler(req, res) {
	var filepath = '.' + req.url;
	if(filepath == './') {
		filepath = './index.html';
	}

	if(req.method == 'POST') {
		console.log('POST START');
		req.on('data', function(chunk) {
			console.log('RECEIVED CHUNK SIZE: ', chunk.length);
			for(var i=0; i<Math.min(chunk.length,20); i+=2) {
				console.log(chunk.readUInt16BE(i));
			}
			//console.log('RECEIVED CHUNK', chunk.toString());
		});
		
		req.on('end', function() {
			console.log('POST END');
			res.writeHead(200, {'Content-Type': 'text/plain'});
			res.end('Post Received\n');
		});
		
		req.on('close', function() {
			console.log('POST CLOSED');
		});
		return;
	}
	
	// Check for static content
	path.exists(filepath, function(exists) {
		if(exists) {
			var contentType = contentTypes[path.extname(filepath)] || 'text/plain';
			fs.readFile(filepath, function(error, content) {
				if(error) {
					res.writeHead(500);
					res.end();
				} else {
					res.writeHead(200, { 'Content-Type': contentType });
					res.end(content, 'utf-8');
				}
			})
		} else {
			// TODO serve up web app commands
			res.writeHead(200, {'Content-Type': 'text/plain'});
			res.end('Hello There!\n');
		}
	})
}
var server = http.createServer(handler).listen(webport, hostname);
console.log('Web Server running at http://' + hostname + ':' + webport + '/');

/**
 Create WebSocket for pushing data to visualizer
 Socket will be available at ://localhost:webport/gio
 **/

var io = require('socket.io').listen(server);
io.set("log level", 0);
var gio = io.of('/gio').on('connection', function(socket) {
	emitStatus('WebSocket connected');
});

function emitStatus(message) {
	gio.emit('giostatus', {
		message: message
	});
	console.log('STATUS: ' + message);
}

var readingsEmitted = 0; //Count how many sets we've sent
function emitData(x, y, z) {
	gio.emit('gio', {
		x: x,
		y: y,
		z: z
	});
	readingsEmitted++;
}

var dataQueue = [];
var queueTimer = -1;

function queueData(x ,y, z) {
	dataQueue.push({
		x: x,
		y: y,
		z: z
	});
}

function checkQueue() {
	if(dataQueue.length) {
		var data = dataQueue.shift();
		emitData(data.x, data.y, data.z);
	}
}

queueTimer = setInterval(checkQueue, Math.floor(1000/sampleDataInterval));

function processHexData(data, emitImmediately) {
	if(!data) return;
	var buffer = new Buffer(6);
	for (var i=0; i < 6; i++) {
		buffer[i] = parseInt('0x' + data.substr(i*2,2), 16);
	}

	function readInt(buffer, offset) {
		return buffer.readInt16BE(offset);
	}

	if(emitImmediately) {
		emitData(readInt(buffer, 0),readInt(buffer, 2),readInt(buffer, 4));
	} else {
		queueData(readInt(buffer, 0),readInt(buffer, 2),readInt(buffer, 4));
	}
}

/*
fs = require('fs');
fs.readFile('pounce.txt', 'utf8', function(err, data) {
	if(err) {
		return console.log(err);
	}
	clearInterval(sampleDataInterval);
	var dataArr = data.split('\n');
	var i=0;
	setInterval(function() {
		var readings = dataArr[i].split(',');
		i = (i+1)%dataArr.length;
		gio.emit('gio', {
			x: readings[0],
			y: readings[1],
			z: readings[2]
		});
	}, 20);
});
*/

// TODO: remove this once we're ready to take in live data
// Randomly generated data
var sampleDataInterval = setInterval(function() {
	emitData(Math.floor(Math.sin(new Date().getTime()/1000 + 1349087) * 16000),
		Math.floor(Math.sin(new Date().getTime()/1000) * 16000),
		Math.floor(Math.sin(new Date().getTime()/1000 + 2349087) * 16000)
	);
}, 50);

/**
 Network server that listens for a stream of data
 **/

net.createServer(function (c) {
	c.write('Catch some Quakes!\r\n');
	c.pipe(c);
	emitStatus('Device Connected');
	c.on('data', function(chunk) {
		//console.log('incoming chunk!', chunk, chunk+'');
		var set = String(chunk).split('\n');
		for(var i=0; i<set.length; i++) {
			processHexData(set[i]);
		}
	});
	c.on('end', function() {
		emitStatus('Device Disconnected');
	})
	clearInterval(sampleDataInterval); // Stop the sample data generator
}).listen(dataport, hostname);
console.log('Data Server listening on ' + hostname + ':' + dataport);

/**
 Serial Port Listener for debugging frame packets from sensor
 **/

try {
	var SerialPort = require("serialport");
	var serialPort = new SerialPort.SerialPort(serialPath, { parser: SerialPort.parsers.readline('\r\n') });

	var buffer = new Buffer(6);
	var bufferIndex = 0;
	var setsReceived = 0;
	serialPort.on("data", function(data) {
		// The assumed format are three hex numbers representing x, y and z
		// example: data = '0035F0230045';
		console.log("Chunk received: " + data.toString(), " Chunk size", data.length);
		data = data.toString();
		processHexData(data);
	}).on("error", function(msg) {
		console.log('Serial Port Error: ' + msg);
	});
	clearInterval(sampleDataInterval); // Stop the sample data generator
} catch(e) {
	console.log('Serial Port Exception: ' + e.toString());
}