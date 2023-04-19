const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

ctx.fillStyle = 'black';
ctx.fillRect(0, 0, canvas.width, canvas.height);

const RES = 64;

(async () => {
    const {instance} = await WebAssembly.instantiateStreaming(fetch('build/main.wasm'));

    // const rm = instance.exports;

    // let arrPointer = instance.exports.returnArrayOfLength(10);
    // let arr = new Uint32Array(instance.exports.memory.buffer, arrPointer, 10);
    // console.log(arr);
    // console.log(instance.exports.getHeapUsage());

    const worldPointer = instance.exports.createWorld();
    console.log(instance.exports);
    let time = performance.now();
    const raymarchResult = instance.exports.rayMarch(RES, worldPointer);

    const raymarchResultArr = new Int8Array(instance.exports.memory.buffer, raymarchResult, RES * RES * 3);

    console.log("calling wasm took: " + (performance.now() - time));
    time = performance.now();

    const pixelSize = canvas.width / RES;
    for (let i = 0; i < RES; i++) {
        for (let j = 0; j < RES; j++) {
            ctx.fillStyle = `rgb(${raymarchResultArr[i * RES + j]}, ${raymarchResultArr[i * RES + j + 1]}, ${raymarchResultArr[i * RES + j + 2]})`;
            ctx.fillRect(j * pixelSize, i * pixelSize, pixelSize - 1, pixelSize - 1);
        }
    }
    console.log("iterating thru pixels took: " + (performance.now() - time));
})();
