function highPassFilter(dataArray, windowSize, scale, negScale) {
    const window = createHighPassWindow(windowSize, scale, negScale);
    return convolution(dataArray, window, Math.floor(window.length / 2) + 1);
}
