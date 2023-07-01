function createFallWindow(windowSize, scale, negScale) {
    const window = [];
    for (let i = 0; i < Math.floor(windowSize / 2); i++) {
        window.push(scale);
    }
    window.push(scale);
    for (let i = 0; i < Math.floor(windowSize / 2); i++) {
        window.push(-1 * scale * negScale);
    }
    return window;
}
