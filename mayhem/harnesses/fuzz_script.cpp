// FooScript (title formatting) scanner/parser/evaluator against a fixed track: ScriptParser::evaluate.
#include "fuzz_common.h"
#include <core/scripting/scriptparser.h>
#include <core/scripting/scriptregistry.h>
#include <core/scripting/scripttypes.h>
#include <core/track.h>

namespace {
Fooyin::Track makeTrack()
{
    Fooyin::Track track{QStringLiteral("/music/Artist/Album/01 - Title.flac")};
    track.setTitle(QStringLiteral("Only Shallow"));
    track.setArtists({QStringLiteral("My Bloody Valentine")});
    track.setAlbum(QStringLiteral("Loveless"));
    track.setTrackNumber(QStringLiteral("1"));
    track.setDuration(257000);
    return track;
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    static const Fooyin::Track track = makeTrack();
    const QString script = QString::fromUtf8(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    // Fresh registry + parser per input: the parser caches parsed scripts, which would make
    // results depend on earlier inputs.
    Fooyin::ScriptRegistry registry;
    registry.finalise();
    Fooyin::ScriptParser parser{&registry};
    // Preserve whitespace mode: the default IgnoreLayout mode has a known out-of-bounds read on any
    // script ending in layout whitespace (ScriptScanner::scanNext's skip loop advances past the
    // terminating NUL, e.g. the 1-byte input "\n"), which would stop every run at once.
    Fooyin::ScriptEvaluationOptions options;
    options.whitespaceMode = Fooyin::ScriptWhitespaceMode::Preserve;
    (void)parser.evaluate(script, track, Fooyin::ScriptContext{}, options);
    return 0;
}
