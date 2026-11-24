// M3uParser::readPlaylist over an in-memory .m3u playlist (the path a user-opened playlist file takes).
#include "fuzz_common.h"
#include "core/playlist/parsers/m3uparser.h"
#include <core/track.h>
#include <QBuffer>
#include <QDir>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    QByteArray bytes(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    Fooyin::PlaylistParser::ReadPlaylistEntry readEntry;
    readEntry.readTrack = [](const Fooyin::Track& track) {
        return track;
    };
    const QString path = QStringLiteral("/nonexistent-fooyin-fuzz/playlist.m3u");
    const QDir dir{QStringLiteral("/nonexistent-fooyin-fuzz")};
    for(const bool skipNotFound : {false, true}) {
        QBuffer buffer{&bytes};
        if(!buffer.open(QIODevice::ReadOnly)) {
            return 0;
        }
        Fooyin::M3uParser parser;
        const auto tracks = parser.readPlaylist(&buffer, path, dir, readEntry, skipNotFound);
        for(const auto& track : tracks) {
            (void)track.duration();
            (void)track.title();
        }
    }
    return 0;
}
