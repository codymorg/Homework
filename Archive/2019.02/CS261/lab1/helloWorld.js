const http = require("http");
const port = 1337;

const server = http.createServer((req, res) => {
    console.log(`Received request from agent ${req.headers['user-agent']}`);
    res.statusCode = 200;
    res.setHeader("Content-Type", "text/plain");
    res.end("Hello World\n");
}).listen(port, () => {
	console.log('started server at ', server.address());
});