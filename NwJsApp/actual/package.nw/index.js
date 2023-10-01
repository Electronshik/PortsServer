const { execFile } = require('child_process');
const port = '80';

const server = execFile('CppServer.exe', ['./', port], (error, stdout, stderr) => {
	if (error) {
	  throw error;
	}
	console.log(stdout);
});

process.on('exit', function (){
    console.log('Goodbye Server!');
	server.kill();
});

// nw.Window.open('index.html');

nw.Window.open('http://localhost:' + port);
