function mean(dataArray, windowSize, scale) {
    const window = createMeanWindow(windowSize, scale);
    return convolution(dataArray, window, Math.floor(window.length / 2) + 1);
}
