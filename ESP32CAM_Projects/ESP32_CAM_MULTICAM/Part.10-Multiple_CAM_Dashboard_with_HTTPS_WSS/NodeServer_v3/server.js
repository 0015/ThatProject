const path = require("path");
const express = require("express");
const WebSocket = require("ws");
const fs = require("fs");
const https = require("https");

var privateKey = fs.readFileSync("server.key", "utf8");
var certificate = fs.readFileSync("server.cert", "utf8");
var credentials = { key: privateKey, cert: certificate };

const HTTPS_PORT = 443;
const app = express();
const httpsServer = https.createServer(credentials, app);
const wsServer = new WebSocket.Server({ server: httpsServer });

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
httpsServer.listen(HTTPS_PORT, () => console.log(`HTTPS server listening at ${HTTPS_PORT}`));
