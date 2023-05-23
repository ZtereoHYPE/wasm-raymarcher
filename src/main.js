const canvas = document.getElementById('canvas');
const gl = canvas.getContext('webgl2');


const RES = 256;

let wasmModule = null;

let program, textureId, fragmentSource, vertexSource;

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
        
        let fragFetch = fetch('glsl/fragment.glsl')
            .then(response => response.text())
            .then(text => fragmentSource = text);

        let vtxFetch = fetch('glsl/vertex.glsl')
            .then(response => response.text())
            .then(text => vertexSource = text);

        Promise.all([fragFetch, vtxFetch]).then(() => {
            setup();
            draw();
        });
    })



let worldPointer = null;
let exp = null;
let pressedKeys = [];


function setup() {
    // setup world 
    exp = wasmModule.exports;
    worldPointer = exp.createWorld();
    exp.addSphere(worldPointer, 0, 0, 20, 15);
    exp.addSphere(worldPointer, -30, 1, 20, 7.0);
    exp.addSphere(worldPointer, -20, 16, 20, 2.0);
    exp.addPlane(worldPointer, 10, 0, 0, 1, 0, 0);

    exp.setLight(worldPointer, -3, -1, -0.5);

    // setup event listeners
    window.addEventListener('keydown', (e) => {
        pressedKeys[e.keyCode] = true;
    });
    window.addEventListener('keyup', (e) => {
        pressedKeys[e.keyCode] = false;
    });

    // setup webgl stuff
    const vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vertexSource);
    gl.compileShader(vertexShader);

    const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fragmentSource);
    gl.compileShader(fragmentShader);

    program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);

    // 2 tris cover entire screen
    let squareVertices = new Float32Array([
        -1, 1,
        -1, -1,
        1, 1,
        -1, -1,
        1, -1,
        1, 1
    ]);
    
    const vertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, squareVertices, gl.STATIC_DRAW);

    const positionAttribLocation = gl.getAttribLocation(program, 'a_position');
    gl.vertexAttribPointer(
        positionAttribLocation,
        2,
        gl.FLOAT,
        gl.FALSE,
        2 * Float32Array.BYTES_PER_ELEMENT,
        0
    );

    gl.enableVertexAttribArray(positionAttribLocation);

    textureId = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, textureId);
    gl.activeTexture(gl.TEXTURE0);
    gl.texImage2D(
        gl.TEXTURE_2D,
        0,
        gl.RGB32F,
        RES,
        RES,
        0,
        gl.RGB,
        gl.FLOAT,
        null
    );
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

    gl.useProgram(program);

    const textureUniformLocation = gl.getUniformLocation(program, 'u_texture');
    gl.uniform1i(textureUniformLocation, 0);
}

function draw() {
    movePlayer();
    
    gl.clearColor(0, 0, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let startTime = performance.now();
    
    const raymarchResult = exp.rayMarch(RES, worldPointer);

    let raymarchTime = performance.now() - startTime;

    const raymarchResultArr = new Float32Array(exp.memory.buffer, raymarchResult, RES * RES * 3);

    // update the texture data with the result
    gl.texSubImage2D(
        gl.TEXTURE_2D,
        0,
        0,
        0,
        RES,
        RES,
        gl.RGB,
        gl.FLOAT,
        raymarchResultArr
    );

    gl.drawArrays(gl.TRIANGLES, 0, 6);

    console.log("FPS: " + Math.floor(1000/(performance.now() - startTime)), "raymarch ratio: " + Math.round((raymarchTime / (performance.now() - startTime)) * 100) / 100);

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