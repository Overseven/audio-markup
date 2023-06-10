function simpleGate(dataArray, startGate, endGate) {
    const result = [];
    let startIdx = -1;
    for (let i = 0; i < dataArray.length; i++) {
        if (startIdx === -1) {
            if (dataArray[i] >= startGate) {
                startIdx = i;
            }
        } else if (dataArray[i] <= endGate) {
            result.push([startIdx, i]);
            startIdx = -1;
        }
    }
    return result;
}
