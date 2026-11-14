#include "query.h"

#include <memory>

std::optional<sword::ListKey> parse_reference(const sword::SWModule* module, const char* reference) {
    if (!module) return std::nullopt;

    // Parse in the module's versification so SWORD does not remap its verse numbers.
    const std::unique_ptr<sword::SWKey> module_key(module->createKey());
    const auto* verse_key = dynamic_cast<const sword::VerseKey*>(module_key.get());
    if (!verse_key) return std::nullopt;

    sword::VerseKey key;
    key.setVersificationSystem(verse_key->getVersificationSystem());
    key.setLocale("ko");
    key.setAutoNormalize(true);
    key.setIntros(true);

    std::optional<sword::ListKey> verses{key.parseVerseList(reference, key, true)};
    if (verses->getCount() == 0) return std::nullopt;
    verses->setPosition(sword::TOP);
    return verses;
}
