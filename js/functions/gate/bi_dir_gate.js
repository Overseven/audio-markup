function biDirGate(dataArray, startGate, endGate) {
    const result = new Array(dataArray.length).fill(0);
    let i = 0;
    while (i < dataArray.length) {
        const val = data[i];
        const maxFindIndex =
            i + searchArea < dataArray.length ? i + searchArea : dataArray.length;
        if (val > posGate) {
            const maxVal = Math.max(...dataArray.slice(i, maxFindIndex));
            const maxValIdx = dataArray.findIndex((x) => x === maxVal);
            result[maxValIdx] = scale;
            i = maxFindIndex;
        } else {
            i++;
        }
    }

    i = 0;
    while (i < dataArray.length) {
        const val = dataArray[i];
        const maxFindIndex =
            i + searchArea < dataArray.length ? i + searchArea : dataArray.length;
        if (val < negGate) {
            const maxVal = Math.min(...dataArray.slice(i, maxFindIndex));
            const minValIdx = dataArray.findIndex((x) => x === maxVal);
            result[minValIdx] = -scale;
            i = maxFindIndex;
        } else {
            i++;
        }
    }
    return result;
}
