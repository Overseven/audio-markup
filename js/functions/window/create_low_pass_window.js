function createLowPassWindow(windowSize, scale) {
    const window = [];
    const foo = (x) => Math.sin(Math.PI * x) / (Math.PI * x);
    const step = 1 / Math.floor(windowSize / 2);
    for (let i = 0; i < Math.floor(windowSize / 2); i++) {
        const x = -1 + i * step;
        window.push(foo(x) * scale);
    }
    window.push(scale);
    for (let i = 0; i < Math.floor(windowSize / 2); i++) {
        const x = (i + 1) * step;
        window.push(foo(x) * scale);
    }
    return window;
}
