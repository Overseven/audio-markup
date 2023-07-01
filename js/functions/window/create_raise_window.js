function createRaiseWindow(windowSize, scale, negScale) {
    const window = [];
    for (let i = 0; i < Math.floor(windowSize / 2); i++) {
        window.push(-1 * scale * negScale);
    }
    window.push(-1 * scale * negScale);
    for (let i = 0; i < Math.floor(windowSize / 2); i++) {
        window.push(scale);
    }
    return window;
}
