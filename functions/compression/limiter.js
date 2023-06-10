function limiter(dataArray, threshold, ratio) {
    const result = [];
    const decrease = (x) => {
        return (x - threshold) / ratio + threshold;
    };
    let thresholdPassed = false;
    for (let i = 0; i < dataArray.length; i++) {
        if (thresholdPassed) {
            if (dataArray[i] < threshold) {
                thresholdPassed = false;
                result.push(dataArray[i]);
            } else {
                result.push(decrease(dataArray[i]));
            }
        } else {
            if (dataArray[i] > threshold) {
                thresholdPassed = true;
                result.push(decrease(dataArray[i]));
            } else {
                result.push(dataArray[i]);
            }
        }
    }
    return result;
}
