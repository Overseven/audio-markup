function derivativeFilter(dataArray, windowSize, scale) {
    const window = createDerivativeWindow(windowSize, scale);
    return convolution(dataArray, window, Math.floor(window.length / 2) + 1);
}
