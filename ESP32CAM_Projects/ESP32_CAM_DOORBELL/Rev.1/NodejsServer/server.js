const fs = require("file-system");
const telegram = require("telegram-bot-api");
const http = require("http");
const dateTime = require("node-datetime");
const fileName = "./resources/test.jpeg";

var telegramAPI = new telegram({
	token: "<PUT YOUR TOKEN HERE>",
	updates: {
		enabled: true
	}
});

function sendPhoto() {
	var dt = dateTime.create();
	var formatted = dt.format("Y-m-d H:M:S");
	telegramAPI
		.sendPhoto({
			chat_id: "<PUT YOUR CHAT ID>",
			caption: formatted,
			photo: fileName
		})
		.then(function (data) {
			console.log(data);
		});
}

const server = http.createServer(function (request, response) {
	if (request.method == "POST" && request.url === "/imageUpdate") {
		var ImageFile = fs.createWriteStream(fileName, { encoding: "utf8" });
		request.on("data", function (data) {
			ImageFile.write(data);
		});

		request.on("end", function () {
			ImageFile.end();
			response.writeHead(200, "OK", { "Content-Type": "text/plain" });
			response.end();
			sendPhoto();
		});
	} else {
		response.writeHead(405, { "Content-Type": "text/plain" });
		response.end();
	}
});

const port = 8888;
server.listen(port);
console.log(`Listening at ${port}`);
