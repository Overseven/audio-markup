function derivativeRaise(dataArray, windowSize, scale, negScale) {
    const window = createRaiseWindow(windowSize, scale, negScale);
    return convolution(dataArray, window, Math.floor(window.length / 2) + 1);
}
