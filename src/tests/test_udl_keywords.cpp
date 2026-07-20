// test_udl_keywords.cpp — verify UDL keyword registry + LanguageManager L_USER lookup
// Copyright (C) 2026 Agent Army — GPL v3
//
// Covers Wave 6 of the npp-qt semantic-lift plan:
//   1. registerUdlKeywords + getUdlKeywordSet + hasUdlKeywords
//   2. setActiveUdlName routes L_USER → registered keyword sets via getKeywords()

#include "../core/LanguageManager.h"
#include <array>
#include <cassert>
#include <iostream>
#include <string>

#define EXPECT(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " << #cond << " at line " << __LINE__ << std::endl; \
        ++failures; \
    } else { \
        ++passed; \
    } \
} while (0)

static int failures = 0;
static int passed   = 0;

static void test_register_and_lookup() {
    std::cout << "test_register_and_lookup... " << std::flush;
    auto& mgr = LanguageManager::instance();

    std::array<QString, 9> sets = {
        QStringLiteral("foo bar baz"),       // words0 — primary
        QStringLiteral("alpha beta"),        // words1
        QStringLiteral("red green blue"),    // words2
        QString(), QString(), QString(),
        QString(), QString(), QString()
    };
    mgr.registerUdlKeywords(QStringLiteral("MyLang"), sets);

    EXPECT(mgr.hasUdlKeywords(QStringLiteral("MyLang")));
    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("MyLang"), 0) == QStringLiteral("foo bar baz"));
    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("MyLang"), 1) == QStringLiteral("alpha beta"));
    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("MyLang"), 2) == QStringLiteral("red green blue"));
    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("MyLang"), 3).isEmpty());

    EXPECT(!mgr.hasUdlKeywords(QStringLiteral("UnknownLang")));
    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("UnknownLang"), 0).isEmpty());

    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("MyLang"), 99).isEmpty());  // OOB safe
    EXPECT(mgr.getUdlKeywordSet(QStringLiteral("MyLang"), -1).isEmpty());

    std::cout << "OK" << std::endl;
}

static void test_hasUdlKeywords_only_true_if_any_set_nonempty() {
    std::cout << "test_hasUdlKeywords_empty_sets... " << std::flush;
    auto& mgr = LanguageManager::instance();
    std::array<QString, 9> allEmpty = {};
    mgr.registerUdlKeywords(QStringLiteral("EmptyLang"), allEmpty);
    EXPECT(!mgr.hasUdlKeywords(QStringLiteral("EmptyLang")));
    std::cout << "OK" << std::endl;
}

static void test_getKeywords_for_L_USER_routes_to_active_udl() {
    std::cout << "test_getKeywords_L_USER_active... " << std::flush;
    auto& mgr = LanguageManager::instance();

    std::array<QString, 9> sets = {
        QStringLiteral("if else endif"),
        QStringLiteral("BEGIN END"),
        QString(),
        QStringLiteral("int8_t int16_t"),
        QString(), QString(), QString(), QString(), QString()
    };
    mgr.registerUdlKeywords(QStringLiteral("Proc"), sets);
    mgr.setActiveUdlName(QStringLiteral("Proc"));

    auto kws = mgr.getKeywords(LangType::L_USER);
    auto it0 = kws.find(0);
    auto it1 = kws.find(1);
    auto it2 = kws.find(2);
    auto it3 = kws.find(3);

    EXPECT(it0 != kws.end() && it0->second.find("if") != std::string::npos);
    EXPECT(it0 != kws.end() && it0->second.find("endif") != std::string::npos);
    EXPECT(it1 != kws.end() && it1->second == "BEGIN END");
    EXPECT(it2 == kws.end());  // empty → not in map
    EXPECT(it3 != kws.end() && it3->second.find("int8_t") != std::string::npos);

    // Clear active UDL → empty result
    mgr.setActiveUdlName(QString());
    auto cleared = mgr.getKeywords(LangType::L_USER);
    EXPECT(cleared.empty());

    std::cout << "OK" << std::endl;
}

static void test_getKeywords_for_L_USER_with_no_active_udl() {
    std::cout << "test_getKeywords_L_USER_no_active... " << std::flush;
    auto& mgr = LanguageManager::instance();
    mgr.setActiveUdlName(QString());
    auto kws = mgr.getKeywords(LangType::L_USER);
    EXPECT(kws.empty());
    std::cout << "OK" << std::endl;
}

static void test_getKeywords_for_L_USER_with_unknown_active_udl() {
    std::cout << "test_getKeywords_L_USER_unknown... " << std::flush;
    auto& mgr = LanguageManager::instance();
    mgr.setActiveUdlName(QStringLiteral("NotRegistered"));
    auto kws = mgr.getKeywords(LangType::L_USER);
    EXPECT(kws.empty());
    std::cout << "OK" << std::endl;
}

int main() {
    test_register_and_lookup();
    test_hasUdlKeywords_only_true_if_any_set_nonempty();
    test_getKeywords_for_L_USER_routes_to_active_udl();
    test_getKeywords_for_L_USER_with_no_active_udl();
    test_getKeywords_for_L_USER_with_unknown_active_udl();

    std::cout << "\nUDL keyword tests: " << passed << " passed, " << failures << " failed" << std::endl;
    return failures == 0 ? 0 : 1;
}
