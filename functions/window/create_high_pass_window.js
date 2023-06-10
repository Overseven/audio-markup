function createHighPassWindow(windowSize, scale, negScale) {
    const window = [];
    for (let i = 1; i <= Math.floor(windowSize / 2); i++) {
        window.push(-Math.sin((Math.PI * i) / windowSize) * negScale);
    }
    window.push(scale);
    for (let i = 1; i <= Math.floor(windowSize / 2); i++) {
        window.push(
            -Math.sin(
                (Math.PI * (i + Math.floor(windowSize / 2))) / windowSize
            ) * negScale
        );
    }
    return window;
}
