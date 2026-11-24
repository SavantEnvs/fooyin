// AccurateRip::parseResponse over a (network-supplied) AccurateRip database response. The expected
// disc id comes from the first input byte's track count, the rest is the response body.
#include "fuzz_common.h"
#include <core/engine/verification/accuraterip.h>
#include <QByteArray>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if(size < 1) {
        return -1;
    }
    const Fooyin::AccurateRip::DiscId expected{
        .id1 = 0x0001b0c4, .id2 = 0x000b6a2a, .cddbId = 0x1a0f2c03, .trackCount = (data[0] % 99) + 1};
    const QByteArray body(reinterpret_cast<const char*>(data + 1), static_cast<qsizetype>(size - 1));
    const auto result = Fooyin::AccurateRip::parseResponse(body, expected);
    if(result) {
        for(const auto& pressing : *result) {
            (void)pressing.size();
        }
    }
    return 0;
}
