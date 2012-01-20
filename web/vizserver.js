/**
 Web server that serves up visualization code for quack catcher network

 1) Install NodeJS: https://github.com/joyent/node/wiki/Installation
 2) run from cmd line: 'node vizserver.js'

 **/

var webport = 8124;
var dataport = 8125;

var http = require('http');
var net = require('net');
var path = require('path');
var fs = require('fs');

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

http.createServer(function (req, res) {
	var filepath = '.' + req.url;
	if(filepath == './') {
		filepath = './index.html';
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
}).listen(webport, "127.0.0.1");
console.log('Web Server running at http://127.0.0.1:8124/');

/**
 Network server that listens for a stream of data
 **/

net.createServer(function (c) {
	c.write('Catch some Quakes!\r\n');
	c.pipe(c);
	console.log('Connected!');
	c.on('data', function(chunk) {
		console.log('incoming chunk!', chunk, chunk+'');
	});
	c.on('end', function() {
		console.log('Connection Ended');
	})
}).listen(dataport, '127.0.0.1');
console.log('Data Server listening on 127.0.0.1:8125');