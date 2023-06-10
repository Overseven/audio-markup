function gatesToSignalForm(gatesArray, signalLength, scale) {
    const result = new Array(signalLength).fill(0);
    for (const [start, end] of gatesArray) {
        const signal = new Array(end - start).fill(scale);
        result.splice(start, signal.length, ...signal);
    }
    return result;
}
