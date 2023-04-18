const http = require('http');
const fs = require('fs');

const host = 'localhost';
const port = 8080;

const server = http.createServer((req, res) => {
    res.statusCode = 200;

    let url = req.url;

    // console.log(url)
    if (url.charAt(0) === '/')
        url = url.substring(1);

    if (!url) 
        url = 'src/index.html';
    else
        url = 'src/' + url;

    switch (url.split('.').pop()) {
        case 'js':
            res.setHeader('Content-Type', 'text/javascript');
            break;
        case 'wasm':
            res.setHeader('Content-Type', 'application/wasm');
            break;
        case 'html':
            res.setHeader('Content-Type', 'text/html');
            break;
        case 'css':
            res.setHeader('Content-Type', 'text/css');
            break;
        default:
            res.write('404');
            res.end();
            return;
    }

    console.log("reading " + url);
    
    fs.readFile(url, (err, data) => {
        if (err) {
            console.log(err);
            res.write('404');
            res.end();
        } else {
            res.write(data);
            res.end();
        }
    });
});

server.listen(port, host, () => {
    console.log(`Server running at http://${host}:${port}/`);
});