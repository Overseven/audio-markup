function reduceDerivativeAttack(dataArray, gate, reduceLen, scale){
    const result = dataArray.map((x) => x);
    let i = 0;
    while (i < dataArray.length) {
        if (dataArray[i] > gate) {
            while (i < dataArray.length && dataArray[i] > 0) {
                i++;
            }
            let j = 0;
            while (i < dataArray.length && j < reduceLen) {
                result[i] = dataArray[i] / scale;
                i++;
                j++;
            }
        }
        i++;
    }
    return result;
}
