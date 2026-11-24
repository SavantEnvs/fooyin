// FFmpegReader (init + readTrack: custom AVIO over a QIODevice, metadata/chapter mapping) and
// FFmpegDecoder (init + a bounded decode loop + seek) on an in-memory media file. The first input
// byte selects the file extension (format hint); the rest is the file content.
#include "fuzz_common.h"
#include "core/engine/input/ffmpeg/ffmpeginput.h"
#include <core/engine/audiobuffer.h>
#include <core/track.h>
#include <QBuffer>
#include <array>

namespace {
constexpr std::array Exts{"mp3", "flac", "ogg", "opus", "m4a", "wav", "aiff", "wv", "ape", "mpc",
                          "wma", "tta", "tak", "mka", "aac", "ac3", "dts", "mp2"};
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if(size < 1) {
        return -1;
    }
    const QString path = QStringLiteral("/nonexistent-fooyin-fuzz/input.%1")
                             .arg(QLatin1String(Exts[data[0] % Exts.size()]));
    QByteArray bytes(reinterpret_cast<const char*>(data + 1), static_cast<qsizetype>(size - 1));

    Fooyin::Track track{path};
    {
        QBuffer buffer{&bytes};
        if(!buffer.open(QIODevice::ReadOnly)) {
            return 0;
        }
        const Fooyin::AudioSource source{.filepath = path, .device = &buffer, .remoteStreamDevice = nullptr};
        Fooyin::FFmpegReader reader;
        if(!reader.init(source) || !reader.readTrack(source, track)) {
            return 0;
        }
    }

    QBuffer buffer{&bytes};
    if(!buffer.open(QIODevice::ReadOnly)) {
        return 0;
    }
    const Fooyin::AudioSource source{.filepath = path, .device = &buffer, .remoteStreamDevice = nullptr};
    Fooyin::FFmpegDecoder decoder;
    if(!decoder.init(source, track, {})) {
        return 0;
    }
    decoder.start();
    for(int i = 0; i < 16; ++i) {
        if(!decoder.readBuffer(8192).isValid()) {
            break;
        }
    }
    decoder.seek(0);
    (void)decoder.readBuffer(8192);
    decoder.stop();
    return 0;
}
