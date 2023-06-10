function lowPassFilter(dataArray, windowSize, scale) {
    const window = createLowPassWindow(windowSize, scale);
    return convolution(dataArray, window, Math.floor(window.length / 2) + 1);
}
