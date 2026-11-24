// Shared setup for the fooyin libFuzzer harnesses: one QCoreApplication per process
// (Qt containers/codecs expect it) and Qt log output silenced. Nothing here is mutated per input.
#pragma once
#include <QCoreApplication>
#include <QtGlobal>
#include <cstddef>
#include <cstdint>

namespace fyfuzz {
inline void quietHandler(QtMsgType, const QMessageLogContext&, const QString&) { }
inline void init()
{
    static int argc = 1;
    static char arg0[] = "fooyin-fuzz";
    static char* argv[] = {arg0, nullptr};
    static QCoreApplication* app = nullptr;
    if(!app) {
        qInstallMessageHandler(quietHandler);
        app = new QCoreApplication(argc, argv);
    }
}
} // namespace fyfuzz

extern "C" int LLVMFuzzerInitialize(int*, char***)
{
    fyfuzz::init();
    return 0;
}
