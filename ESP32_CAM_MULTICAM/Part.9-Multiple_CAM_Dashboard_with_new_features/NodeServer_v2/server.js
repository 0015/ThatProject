const path = require("path");
const express = require("express");
const WebSocket = require("ws");
const app = express();

const WS_PORT = 8888;
const HTTP_PORT = 8000;

const wsServer = new WebSocket.Server({ port: WS_PORT }, () => console.log(`WS Server is listening at ${WS_PORT}`));

let connectedClients = [];
wsServer.on("connection", (ws, req) => {
	console.log("Connected");

	ws.on("message", (data) => {
		if (data.indexOf("WEB_CLIENT") !== -1) {
			connectedClients.push(ws);
			console.log("WEB_CLIENT ADDED");
			return;
		}

		connectedClients.forEach((ws, i) => {
			if (connectedClients[i] == ws && ws.readyState === ws.OPEN) {
				ws.send(data);
			} else {
				connectedClients.splice(i, 1);
			}
		});
	});

	ws.on("error", (error) => {
		console.error("WebSocket error observed: ", error);
	});
});

app.use(express.static("."));
app.get("/client", (req, res) => res.sendFile(path.resolve(__dirname, "./client.html")));
app.listen(HTTP_PORT, () => console.log(`HTTP server listening at ${HTTP_PORT}`));
