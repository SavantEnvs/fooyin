// Lyrics plugin parser (LRC / enhanced LRC / plain text) — the public Lyrics::parse entry points.
#include "fuzz_common.h"
#include "lyricsparser.h"
#include <QByteArray>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    const QByteArray bytes(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    const auto fromBytes  = Fooyin::Lyrics::parse(bytes);
    const auto fromString = Fooyin::Lyrics::parse(QString::fromUtf8(bytes));
    (void)fromBytes;
    (void)fromString;
    (void)Fooyin::Lyrics::timestampToMs(QString::fromUtf8(bytes.left(64)));
    return 0;
}
