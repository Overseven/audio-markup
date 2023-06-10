function convolution(dataArray, window, windowMiddleIdx) {
    const result = [];
    if (dataArray.length < window.length) {
        throw new Error('convolution: dataArray.length < window.length');
    }
    if (window.length % 2 !== 1) {
        throw new Error('convolution: window.length % 2 !== 1');
    }
    let i = 0;
    for (; i < windowMiddleIdx; i++) {
        result.push(0);
    }
    for (; i < dataArray.length - windowMiddleIdx; i++) {
        let sum = 0;
        for (let j = 0; j < window.length; j++) {
            const idx = i - windowMiddleIdx + j;
            sum += dataArray[idx] * window[j];
        }
        result.push(sum / window.length);
    }
    for (; i < dataArray.length; i++) {
        result.push(0);
    }
    return result;
}
