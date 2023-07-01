function createMeanWindow(windowSize, scale) {
    const window = [];
    for (let i = 0; i < windowSize; i++) {
        window.push(scale);
    }
    return window;
}
