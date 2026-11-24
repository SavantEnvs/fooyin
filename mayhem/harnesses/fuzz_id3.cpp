// Timed ID3 metadata (from HLS emsg / stream frames) and the MPEG frame/Xing header scanner.
#include "fuzz_common.h"
#include "core/engine/input/metadata/id3.h"
#include "core/engine/input/metadata/mpegproperties.h"
#include <core/track.h>
#include <QByteArrayView>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    const QByteArrayView view(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    (void)Fooyin::Metadata::parseTimedId3Metadata(view);
    Fooyin::Track track;
    Fooyin::Metadata::readMpegProperties(view, track);
    (void)track.duration();
    return 0;
}
