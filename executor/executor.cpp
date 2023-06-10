#include "executor.h"
#include <QDebug>

Executor::Executor()
{
    QJSEngine engine;
    QString functions = "function mull(a, b) {return a * b;} function summ(a, b) {return a + b;}";
    QString call = "summ(mull(10,4), 100)";
    QJSValue result = engine.evaluate(functions + call);

    qDebug() << "js result: " << result.toNumber();
}
