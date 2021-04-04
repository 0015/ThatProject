/////////////////////////////////////////////////////////////////
/*
  ESP32 | Walkie-Talkie based on Node.js Server for multi-clients (ft. PCM speaker)
  Video Tutorial: https://youtu.be/vq7mPgecGKA
  Created by Eric Nam (ThatProject)
*/
/////////////////////////////////////////////////////////////////

////////////
// States
const states = {
	IDLE: "Idle",
	LISTENING: "Listening",
	SPEAKING: "Speaking"
};

////////////
// Speaker(Optional)
// MacOS Issue
// [../deps/mpg123/src/output/coreaudio.c:81] warning: Didn't have any audio data in callback (buffer underflow)
// https://github.com/audiojs/audio-play/issues/15
const Speaker = require("speaker");
const Readable = require("stream").Readable;
const pcmSpeaker = new Readable();
pcmSpeaker.bitDepth = 32;
pcmSpeaker.channels = 1;
pcmSpeaker.sampleRate = 16000;
pcmSpeaker._read = bufRead;
pcmSpeaker.pipe(new Speaker());
function bufRead(buf) {
	if (buf instanceof Buffer) {
		pcmSpeaker.resume();
		pcmSpeaker.push(buf);
	} else if (buf == null) {
		pcmSpeaker.pause();
	}
}

////////////
// WebSocket
const WebSocket = require("ws");
const WS_PORT = process.env.WS_PORT || 8888;
const wsServer = new WebSocket.Server(
	{
		port: WS_PORT,
		perMessageDeflate: {
			concurrencyLimit: 10,
			threshold: 1024
		}
	},
	() => console.log(`[Server] WS server is listening at ${WS_PORT}`)
);

wsServer.on("connection", (ws, req) => {
	console.log("[Client] Connected!");
	ws.timestamp = getTimestamp();
	ws.id = wsServer.getUniqueID(ws.timestamp);
	ws.state = states.IDLE;

	ws.send(ws.timestamp);
	ws.on("message", (event) => {
		messageHandler(ws, event);
	});
});

wsServer.getUniqueID = function (timestamp) {
	function s4() {
		return Math.floor((1 + Math.random()) * 0x10000)
			.toString(16)
			.substring(1);
	}
	return timestamp + "-" + s4();
};

function messageHandler(ws, event) {
	ws.timestamp = getTimestamp();
	if (event instanceof Buffer) {
		if (event.length == 1) {
			console.log("[Client] Ping from: " + ws.id);
		} else if (event.length > 1) {
			sendDataClients(ws, event);
			bufRead(event); // Resume Speaker Out
		}
	} else {
		if (checkAllClientState()) {
			ws.state = states.SPEAKING;
			ws.send(event);
		} else {
			ws.state = states.IDLE;
			bufRead(null); // Pause Speaker Out
		}
		sendDataClients(ws, event);
	}
}

function sendDataClients(d_client, data) {
	var timestamp = getTimestamp();
	wsServer.clients.forEach(function each(client) {
		if (d_client != client) {
			client.timestamp = timestamp;
			console.log("[Client] Client Id: " + client.id + " Client State: " + client.state);
			if (client.state == states.IDLE) {
				// OTHER CLIENTS, IDLE -> LISTENING
				if (data instanceof Buffer) {
					// NO ACTION REQUIRED
				} else {
					client.state = states.LISTENING;
					client.send(timestamp);
				}
			} else {
				// OTHER CLIENTS, LISTENING -> IDLE
				if (data instanceof Buffer) {
					client.send(data);
				} else {
					client.state = states.IDLE;
					client.send(timestamp);
				}
			}
		}
	});
}

function getTimestamp() {
	var last8digit_Timestamp = Date.now().toString().slice(-8);
	return parseInt(last8digit_Timestamp);
}

function checkAllClientState() {
	var result = false;
	wsServer.clients.forEach(function each(client) {
		if (client.state != states.IDLE) {
			result = false;
			return;
		}
		result = true;
	});
	return result;
}

function checkAlive() {
	console.log("[Server] Total number of connected clients: " + wsServer.clients.size);
	wsServer.clients.forEach(function each(client) {
		if (client.state == states.IDLE) {
			console.log(
				"[Check Clinet Alive] Client Id: " +
					client.id +
					" Timestamp DIFF: " +
					(getTimestamp() - client.timestamp)
			);
			if (getTimestamp() - client.timestamp > 10000) {
				// 10sec
				console.log("[Client TimeOut] Client Id: " + client.id);
				client.close;
				wsServer.clients.delete(client);
			}
		}
	});
}

setInterval(function () {
	checkAlive();
}, 5000);
