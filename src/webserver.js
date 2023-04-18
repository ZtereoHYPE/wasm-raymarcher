const http = require('http');
const fs = require('fs');
const { exec } = require('child_process');

const host = 'localhost';
const port = 8080;

// watch the file in src/wasm/ for changes
// and recompile it
const buildCmd = `clang \
            --target=wasm32 \
            -O3 \
            -flto \
            -nostdlib \
            -Wl,--no-entry \
            -Wl,--export-all \
            -Wl,--lto-O3 \
            -Wl,-z,stack-size=$[8 * 1024 * 1024] \
            -o src/build/main.wasm \
            -msimd128 \
            src/wasm/main.c`;

let lastWatch = Date.now();
fs.watch('src/wasm/', {}, (evt, name) => {
    if (Date.now() - lastWatch < 100) return;
    lastWatch = Date.now();

    console.log('%s changed.', name);
    
    exec(buildCmd, (err, stdout, stderr) => {
        if (err) {
            console.error(err);
            return;
        }
        
        console.log(stderr.trim() ? stderr : 'Compiled successfully.')
    });
});

const server = http.createServer((req, res) => {
    res.statusCode = 200;

    let url = req.url;

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