function derivative(dataArray) {
    const result = [0];
    for (let i = 1; i < dataArray.length; i++) {
        result.push(dataArray[i] - dataArray[i - 1]);
    }
    return result;
}
