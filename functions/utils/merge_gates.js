function mergeGates(startGatesArray, endGatesArray) {
    const result = [];
    const g1 = startGatesArray.map(([start, _end]) => start);
    const g2 = endGatesArray.map(([start, _end]) => start);

    while (g1.length > 0 && g2.length > 0) {
        while (g2[0] < g1[0]) {
            g2.splice(0, 1);
        }
        if (g2.length === 0) {
            break;
        }
        result.push([g1[0], g2[0]]);
        g1.splice(0, 1);
        g2.splice(0, 1);
    }
    return result;
}
