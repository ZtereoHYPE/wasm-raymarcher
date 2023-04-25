const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d', {alpha: false});

ctx.fillStyle = 'black';
ctx.fillRect(0, 0, canvas.width, canvas.height);

const RES = 64;

let wasmModule = null;

const memory = new WebAssembly.Memory({initial: 1, maximum: 16});
fetch('build/main.wasm')
    .then(response => response.arrayBuffer())
    .then(buffer => WebAssembly.compile(buffer))
    .then(compiledModule => {
        const jslog = (offset, length) => {
            // let bytes = new Uint8Array(memory.buffer, offset, length);
        
            // let string = new TextDecoder('utf8').decode(char);
        
            console.log(String.fromCharCode(offset));
        }

        wasmModule = new WebAssembly.Instance(compiledModule, {
            js: {mem: memory},
            env: { "jslog": jslog }
        });
        
        setup();
        draw();
    })

let worldPointer = null;
let exp = null;

function setup() {
    exp = wasmModule.exports;
    worldPointer = exp.createWorld();
    exp.addSphere(worldPointer, 0, 0, 10, 0.0);
}

function draw() {
    const raymarchResult = exp.rayMarch(RES, worldPointer);
    const raymarchResultArr = new Int32Array(exp.memory.buffer, raymarchResult, RES * RES * 3);

    const pixelSize = canvas.width / RES;
    for (let i = 0; i < RES; i++) {
        for (let j = 0; j < RES; j++) {
            ctx.fillStyle = `rgb(${raymarchResultArr[(i * RES + j) * 3]}, ${raymarchResultArr[(i * RES + j) * 3 + 1]}, ${raymarchResultArr[(i * RES + j) * 3 + 2]})`;
            ctx.fillRect(j * pixelSize + 1, i * pixelSize + 1, pixelSize - 1, pixelSize - 1);
        }
    }

    // requestAnimationFrame(draw);
}