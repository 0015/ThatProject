var fs = require('file-system');
const http = require('http');
const server = http.createServer();
const filePath = './resources/test.jpeg';

server.on('request', (request, response)=>{
	if(request.method == 'POST' && request.url === "/imageUpdate"){
		
		var ImageFile = fs.createWriteStream(filePath, {encoding: 'utf8'});
		request.on('data', function(data){
			ImageFile.write(data);
		});

		request.on('end',async function(){
			ImageFile.end();
			const labels = await labelAPI();
			response.writeHead(200, {'Content-Type' : 'application/json'});
			response.end(JSON.stringify(labels));
		});


	}else{
		console.log("error");
		response.writeHead(405, {'Content-Type' : 'text/plain'});
		response.end();
	}
});

async function labelAPI() {
  // Imports the Google Cloud client library
  const vision = require('@google-cloud/vision');

  // Creates a client
  const client = new vision.ImageAnnotatorClient();

  // Performs label detection on the image file
  const [result] = await client.labelDetection(filePath);
  const labels = result.labelAnnotations;
  var o = [];
  labels.forEach(label => {
  	o.push({description: label.description, score: label.score});
  });
  return o;
}

const port = 8888;
server.listen(port)
console.log(`Listening at ${port}`)


