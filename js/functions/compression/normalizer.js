function normalizer(dataArray, threshold, noiseThreshold, decreaseRatio, increaseRatio) {
    if (increaseRatio > 10) {
        throw new Error(`normalizer: increaseRatio can't be more that 10`);
    }
    const result = [];
    const decrease = (x) => {
        return (x - threshold) / decreaseRatio + threshold;
    };
    const increase = (x) => {
        return ((threshold - x) / 10) * increaseRatio + x;
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
            } else if (dataArray[i] > noiseThreshold) {
                result.push(increase(dataArray[i]));
            } else {
                result.push(dataArray[i]);
            }
        }
    }
    return result;
}
