// TagLibReader: init + readTrack + readCover on an in-memory media file. The reader picks the
// container by the file name's extension, so the first input byte selects the extension and the
// rest is the file content.
#include "fuzz_common.h"
#include "core/engine/input/taglibparser.h"
#include <core/track.h>
#include <QBuffer>
#include <array>

namespace {
constexpr std::array Exts{"mp3", "flac", "ogg", "opus", "m4a", "wav", "aiff", "wv", "ape",
                          "mpc", "wma", "dsf", "dff", "tta", "spx", "mp4", "oga", "aac"};
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if(size < 1) {
        return -1;
    }
    const QString path = QStringLiteral("/nonexistent-fooyin-fuzz/input.%1")
                             .arg(QLatin1String(Exts[data[0] % Exts.size()]));
    QByteArray bytes(reinterpret_cast<const char*>(data + 1), static_cast<qsizetype>(size - 1));
    QBuffer buffer{&bytes};
    if(!buffer.open(QIODevice::ReadOnly)) {
        return 0;
    }
    const Fooyin::AudioSource source{.filepath = path, .device = &buffer, .remoteStreamDevice = nullptr};
    Fooyin::TagLibReader reader;
    if(!reader.init(source)) {
        return 0;
    }
    Fooyin::Track track{path};
    if(reader.readTrack(source, track)) {
        buffer.seek(0);
        (void)reader.readCover(source, track, Fooyin::Track::Cover::Front);
    }
    return 0;
}
