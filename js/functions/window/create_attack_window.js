function createAttackWindow(preAttackLen, preAttackScale, attackLen, attackScale, decayLen) {
    const window = [];
    for (let i = 0; i < preAttackLen; i++) {
        window.push(-1 * preAttackScale);
    }
    for (let i = 0; i < attackLen; i++) {
        window.push(attackScale);
    }
    const scale = decayLen * decayLen;
    for (let i = decayLen; i >= 0; i--) {
        window.push(((i * i) / scale) * attackScale);
    }
    return window;
}
