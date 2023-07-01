function attackFilter(dataArray, preAttackLen, preAttackScale, attackLen, attackScale, decayLen) {
    const window = createAttackWindow(preAttackLen, preAttackScale, attackLen, attackScale, decayLen);
    return convolution(dataArray, window, preAttackLen);
}
