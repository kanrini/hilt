#include "query.h"

std::optional<sword::ListKey> parse_reference(const sword::SWModule* module, const char* reference) {
    if (!module) return std::nullopt;

    sword::VerseKey key;
    key.setLocale("ko");
    key.setAutoNormalize(true);
    key.setIntros(true);

    std::optional<sword::ListKey> verses{key.parseVerseList(reference, key, true)};
    if (verses->getCount() == 0) return std::nullopt;
    verses->setPosition(sword::TOP);
    return verses;
}
