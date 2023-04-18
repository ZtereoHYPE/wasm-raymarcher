const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

ctx.fillStyle = 'black';
ctx.fillRect(0, 0, canvas.width, canvas.height);

(async () => {
    const {instance} = await WebAssembly.instantiateStreaming(fetch('build/main.wasm'));
    console.log(instance.exports.sumInts(4, 1));
})();
