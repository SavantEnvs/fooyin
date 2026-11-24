// sndfile input plugin (compiled in from src/plugins/sndfile): SndFileReader::readTrack and the
// SndFileDecoder init/readBuffer/seek loop over an in-memory file read through libsndfile's virtual IO.
#include "fuzz_common.h"
#include "sndfileinput.h"
#include <core/track.h>
#include <QBuffer>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    const QString path = QStringLiteral("/nonexistent-fooyin-fuzz/input.wav");
    QByteArray bytes(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    Fooyin::Track track{path};
    {
        QBuffer buffer{&bytes};
        if(!buffer.open(QIODevice::ReadOnly)) {
            return 0;
        }
        const Fooyin::AudioSource source{.filepath = path, .device = &buffer, .remoteStreamDevice = nullptr};
        Fooyin::Snd::SndFileReader reader;
        if(!reader.readTrack(source, track)) {
            return 0;
        }
    }
    QBuffer buffer{&bytes};
    if(!buffer.open(QIODevice::ReadOnly)) {
        return 0;
    }
    const Fooyin::AudioSource source{.filepath = path, .device = &buffer, .remoteStreamDevice = nullptr};
    Fooyin::Snd::SndFileDecoder decoder;
    if(!decoder.init(source, track, {})) {
        return 0;
    }
    decoder.start();
    for(int i = 0; i < 8; ++i) {
        const auto buf = decoder.readBuffer(4096);
        if(!buf.isValid()) {
            break;
        }
    }
    decoder.seek(0);
    (void)decoder.readBuffer(4096);
    decoder.stop();
    return 0;
}
