function createDerivativeWindow(windowSize, scale) {
    if (windowSize < 2) {
        throw new Error(`createDerivativeWindow2: windowSize < 2`);
    } else if (windowSize === 2) {
        return [-scale, scale];
    }
    const halfWindowLen = Math.floor(windowSize / 2);
    const window = [];
    for (let i = halfWindowLen; i >= 1; i--) {
        window.push(-scale * (i / halfWindowLen));
    }
    window.push(0);
    for (let i = 1; i <= halfWindowLen; i++) {
        window.push(scale * (i / halfWindowLen));
    }
    return window;
}
