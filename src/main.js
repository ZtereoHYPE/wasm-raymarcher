const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d', {alpha: false, antialias: false});

ctx.fillStyle = 'black';
ctx.fillRect(0, 0, canvas.width, canvas.height);

const RES = 128;

let wasmModule = null;

const memory = new WebAssembly.Memory({initial: 1, maximum: 16});
fetch('build/main.wasm')
    .then(response => response.arrayBuffer())
    .then(buffer => WebAssembly.compile(buffer))
    .then(compiledModule => {
        const logNum = (n) => {
            console.log(n);
        }

        wasmModule = new WebAssembly.Instance(compiledModule, {
            js: {mem: memory},
            env: { "jslog": logNum }
        });
        
        setup();
        draw();
    })

let worldPointer = null;
let exp = null;
let pressedKeys = [];

function setup() {
    exp = wasmModule.exports;
    worldPointer = exp.createWorld();
    exp.addSphere(worldPointer, 0, 0, 20, 10.0);

    window.addEventListener('keydown', (e) => {
        pressedKeys[e.keyCode] = true;
    });
    window.addEventListener('keyup', (e) => {
        pressedKeys[e.keyCode] = false;
    });
}

function draw() {
    movePlayer();

    let time = performance.now();

    const raymarchResult = exp.rayMarch(RES, worldPointer);
    const raymarchResultArr = new Int32Array(exp.memory.buffer, raymarchResult, RES * RES * 3);
    const pixelSize = canvas.width / RES;
    for (let i = 0; i < RES; i++) {
        for (let j = 0; j < RES; j++) {
            ctx.fillStyle = `rgb(${raymarchResultArr[(i * RES + j) * 3]}, ${raymarchResultArr[(i * RES + j) * 3 + 1]}, ${raymarchResultArr[(i * RES + j) * 3 + 2]})`;
            ctx.fillRect(j * pixelSize, i * pixelSize, pixelSize, pixelSize);
        }
    }

    console.log("FPS: " + Math.floor(1000/(performance.now() - time)));

    requestAnimationFrame(draw);
}

// move player using wasd
function movePlayer() {
    const speed = 1;

    if (pressedKeys[87]) {
        exp.moveCamera(worldPointer, 0, 0, speed);
    }
    if (pressedKeys[83]) {
        exp.moveCamera(worldPointer, 0, 0, -speed);
    }
    if (pressedKeys[65]) {
        exp.moveCamera(worldPointer, 0, -speed, 0);
    }
    if (pressedKeys[68]) {
        exp.moveCamera(worldPointer, 0, speed, 0);
    }
    if (pressedKeys[81]) {
        exp.moveCamera(worldPointer, speed, 0, 0);
    }
    if (pressedKeys[69]) {
        exp.moveCamera(worldPointer, -speed, 0, 0);
    }
}