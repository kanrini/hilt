#include "query.h"

#include <cstdlib>
#include <iostream>
#include <string_view>

int main() {
    sword::SWMgr manager;
    auto* module = manager.getModule("KJV");
    if (!module) {
        std::cerr << "The KJV SWORD module is required for these tests.\n";
        return EXIT_FAILURE;
    }

    auto verse = parse_reference(module, "John 3:16");
    if (!verse || verse->popError()) {
        std::cerr << "Could not parse John 3:16.\n";
        return EXIT_FAILURE;
    }
    module->setKey(*verse);
    if (std::string_view(module->stripText()).find("God") == std::string_view::npos) {
        std::cerr << "Unexpected text for John 3:16.\n";
        return EXIT_FAILURE;
    }

    auto range = parse_reference(module, "Rev 7:1-3");
    if (!range) {
        std::cerr << "Could not parse Rev 7:1-3.\n";
        return EXIT_FAILURE;
    }
    int count = 0;
    for (; !range->popError(); ++*range) {
        module->setKey(*range);
        const auto* key = dynamic_cast<const sword::VerseKey*>(module->getKey());
        if (!key || key->getVerse() != ++count) {
            std::cerr << "Unexpected verse number in Rev 7:1-3.\n";
            return EXIT_FAILURE;
        }
    }
    if (count != 3 || parse_reference(manager.getModule("GALAXY_VERSION"), "John 3:16")) {
        std::cerr << "Unexpected range length or result for a missing module.\n";
        return EXIT_FAILURE;
    }

    if (auto* korrv = manager.getModule("KorRV")) {
        for (const char* reference : {"Rev 13:1", "Rev 13:"}) {
            auto result = parse_reference(korrv, reference);
            if (!result || result->popError()) {
                std::cerr << "Could not parse KorRV " << reference << ".\n";
                return EXIT_FAILURE;
            }
            korrv->setKey(*result);
            const auto* key = dynamic_cast<const sword::VerseKey*>(korrv->getKey());
            if (!key || key->getChapter() != 13 || key->getVerse() != 1 ||
                std::string_view(korrv->stripText()).find_first_not_of(" \t\r\n") == std::string_view::npos) {
                std::cerr << "Expected the first verse of KorRV Revelation 13 for " << reference << ".\n";
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "All tests passed.\n";
}
