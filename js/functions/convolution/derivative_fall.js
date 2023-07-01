function derivativeFall(dataArray, windowSize, scale, negScale) {
    const window = createFallWindow(windowSize, scale, negScale);
    return convolution(dataArray, window, Math.floor(window.length / 2) + 1);
}
