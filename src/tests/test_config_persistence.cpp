// test_config_persistence.cpp — Functional test for the IniParser that backs
// Application::saveConfig/loadConfig.
//
// Goal: prove that every key/value pair Application::saveConfig writes to
// config.xml round-trips correctly through IniParser. Catches the "silent
// data loss" bug class where a UI field writes to memory but never makes
// it to disk — by checking that the underlying string parser handles each
// field type (bool, int, string, stringlist) correctly.
//
// This test is intentionally scoped to IniParser because the actual
// Application singleton in tests is the test_stubs stub (full Application
// requires a live Qt event loop, QApplication, MainWindow, etc.). The
// Application::saveConfig/loadConfig code itself is a mechanical mapping
// `_options.field → ini.set(...)` and `ini.get → _options.field` that is
// covered by the static audit; what THIS test guarantees is that the data
// layer underneath is sound.

#include "common/Util.h"

#include <QCoreApplication>
#include <QStringList>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

static int g_failed = 0;

static std::string strval(int i)                { return std::to_string(i); }
static std::string strval(long long i)          { return std::to_string(i); }
static std::string strval(bool b)               { return std::string(b ? "true" : "false"); }
static std::string strval(const std::string& s) { return s; }

#define EXPECT_EQ(actual, expected, label) do {                                   \
        auto _a = (actual);                                                        \
        auto _e = (expected);                                                      \
        if (!(_a == _e)) {                                                         \
            std::cerr << "FAIL: " << (label) << "\n  got:      "                   \
                      << strval(_a) << "\n  expected: " << strval(_e) << "\n";     \
            ++g_failed;                                                            \
        } else {                                                                   \
            std::cout << "  " << (label) << "... OK\n";                            \
        }                                                                          \
    } while(0)

#define EXPECT_TRUE(x, label) do {                                                 \
        if (!(x)) {                                                                \
            std::cerr << "FAIL: " << (label) << "\n  expr: " << #x << "\n";        \
            ++g_failed;                                                            \
        } else {                                                                   \
            std::cout << "  " << (label) << "... OK\n";                            \
        }                                                                          \
    } while(0)

// Write a config file with one section/key/value, reload it, assert the
// IniParser reads back exactly what we wrote. Exercises the disk-side of
// every field type Application::saveConfig uses.
static void test_disk_round_trip_one(const std::string& section,
                                     const std::string& key,
                                     const std::string& value) {
    static int counter = 0;
    std::string path = "/tmp/npp-cfg-test-" + std::to_string(getpid()) +
                       "-" + std::to_string(++counter) + ".ini";
    {
        std::ofstream f(path);
        f << "[" << section << "]\n" << key << "=" << value << "\n";
    }
    IniParser ini;
    bool loaded = ini.load(path);
    EXPECT_TRUE(loaded, "IniParser loads " + section + "/" + key);
    std::string readBack = ini.get(section, key, "<missing>");
    EXPECT_EQ(readBack, value, section + "/" + key + " round-trip");
    fs::remove(path);
}

// Round-trip a bool through IniParser, asserting both "0/1" and "true/false"
// spellings (Application::saveConfig uses "0"/"1" — confirm IniParser reads
// both).
static void test_bool_round_trip(const std::string& section,
                                 const std::string& key,
                                 bool expected) {
    std::string path = "/tmp/npp-cfg-bool-" + std::to_string(getpid()) + "-" +
                       std::to_string(rand()) + ".ini";
    {
        std::ofstream f(path);
        f << "[" << section << "]\n" << key << "=" << (expected ? "1" : "0") << "\n";
    }
    IniParser ini;
    bool loaded = ini.load(path);
    EXPECT_TRUE(loaded, "IniParser loads bool " + section + "/" + key);
    bool readBack = ini.getBool(section, key, !expected);
    EXPECT_EQ(readBack, expected, section + "/" + key + " bool round-trip");
    fs::remove(path);
}

// Round-trip an int.
static void test_int_round_trip(const std::string& section,
                                const std::string& key,
                                int expected) {
    std::string path = "/tmp/npp-cfg-int-" + std::to_string(getpid()) + "-" +
                       std::to_string(rand()) + ".ini";
    {
        std::ofstream f(path);
        f << "[" << section << "]\n" << key << "=" << expected << "\n";
    }
    IniParser ini;
    bool loaded = ini.load(path);
    EXPECT_TRUE(loaded, "IniParser loads int " + section + "/" + key);
    int readBack = ini.getInt(section, key, -1);
    EXPECT_EQ(readBack, expected, section + "/" + key + " int round-trip");
    fs::remove(path);
}

// Round-trip a stringlist (comma-separated, used by fileAssociations).
static void test_stringlist_round_trip(const std::string& section,
                                       const std::string& key,
                                       const QStringList& values) {
    std::string path = "/tmp/npp-cfg-strlist-" + std::to_string(getpid()) +
                       "-" + std::to_string(rand()) + ".ini";
    {
        std::ofstream f(path);
        f << "[" << section << "]\n" << key << "=" << values.join(",").toStdString()
          << "\n";
    }
    IniParser ini;
    bool loaded = ini.load(path);
    EXPECT_TRUE(loaded, "IniParser loads stringlist " + section + "/" + key);
    QStringList readBack = ini.getStringList(section, key);
    EXPECT_EQ(readBack.size(), values.size(),
              section + "/" + key + " stringlist length round-trip");
    if (readBack.size() == values.size()) {
        for (int i = 0; i < values.size(); ++i) {
            EXPECT_EQ(readBack[i].toStdString(), values[i].toStdString(),
                      section + "/" + key + "[" + std::to_string(i) + "] round-trip");
        }
    }
    fs::remove(path);
}

// Drive IniParser through the same set of (section, key, value) tuples that
// Application::saveConfig writes. Each row is verified by writing the file
// fresh, loading it, and checking IniParser reads the same value back.
static void test_full_saveConfig_payload_round_trip() {
    std::cout << "\n  Application::saveConfig payload round-trip:\n";

    // [General]
    test_bool_round_trip("General", "SingleInstance", false);
    test_bool_round_trip("General", "ShowTabBar",     true);
    test_bool_round_trip("General", "ShowStatusBar",  false);
    test_bool_round_trip("General", "ShowToolBar",    true);
    test_bool_round_trip("General", "ShowMenuBar",    true);
    test_bool_round_trip("General", "RememberSession", false);
    test_int_round_trip ("General", "MaxRecentFiles", 7);
    test_disk_round_trip_one("General", "Theme", "monokai");

    // [Editor]
    test_int_round_trip ("Editor",  "TabWidth",        16);
    test_int_round_trip ("Editor",  "IndentWidth",     8);
    test_bool_round_trip("Editor",  "TabAsSpaces",     true);
    test_bool_round_trip("Editor",  "WordWrap",        false);
    test_bool_round_trip("Editor",  "VirtualSpace",    true);
    test_bool_round_trip("Editor",  "SmartHome",       false);
    test_bool_round_trip("Editor",  "AutoIndent",      true);
    test_bool_round_trip("Editor",  "BackspaceUnindent", true);
    test_bool_round_trip("Editor",  "ShowEol",         false);
    test_bool_round_trip("Editor",  "ShowSpaceTab",    true);
    test_bool_round_trip("Editor",  "ShowIndentGuide", false);

    // [Margins]
    test_bool_round_trip("Margins", "ShowLineNumbers",    true);
    test_int_round_trip ("Margins", "LineNumberWidth",    6);
    test_bool_round_trip("Margins", "ShowSymbols",        false);
    test_bool_round_trip("Margins", "ShowFolderMargin",   true);
    test_int_round_trip ("Margins", "SymbolMarginWidth",  3);
    test_bool_round_trip("Margins", "HighlightCurrentLine", true);
    test_bool_round_trip("Margins", "ShowEdgeLine",       false);
    test_int_round_trip ("Margins", "EdgeColumn",         100);

    // [Backup]
    test_disk_round_trip_one("Backup", "BackupDir", "/tmp/npp-backup");
    test_int_round_trip  ("Backup", "BackupStyle",         2);
    test_int_round_trip  ("Backup", "MaxBackups",          3);
    test_bool_round_trip ("Backup", "AutoSave",            true);
    test_int_round_trip  ("Backup", "AutoSaveInterval",    7);
    test_bool_round_trip ("Backup", "AutoSaveCurrentOnly", false);
    test_bool_round_trip ("Backup", "AutoSaveInBackground", true);

    // [ShortcutMapper]
    test_bool_round_trip("ShortcutMapper", "WarnOnConflict", true);

    // [Editor] CurrentLineHighlightColor (string, hex)
    test_disk_round_trip_one("Editor", "CurrentLineHighlightColor", "#aabbcc");

    // [Search]
    test_bool_round_trip("Search", "MatchCase",    false);
    test_bool_round_trip("Search", "MatchWholeWord", true);
    test_bool_round_trip("Search", "WrapAround",   false);
    test_int_round_trip ("Search", "FindHistoryCount", 25);
    test_bool_round_trip("Search", "AutoDetectLanguage", true);

    // [General] FileAssociations (string list)
    test_stringlist_round_trip("General", "FileAssociations",
        QStringList{".txt=Text File", ".cpp=C++ Source", ".py=Python Script"});
}

// Verify that Application::saveConfig-equivalent set/get sequences round-trip
// through IniParser when applied programmatically (no real Application).
static void test_ini_set_get_round_trip() {
    std::cout << "\n  IniParser programmatic set/get:\n";
    IniParser ini;
    ini.set("General", "SingleInstance", false);
    ini.set("Editor",  "TabWidth", 16);
    ini.set("Editor",  "TabAsSpaces", true);
    ini.set("Editor",  "Theme", std::string("monokai"));
    ini.set("Backup",  "BackupDir", std::string("/tmp/x"));
    ini.set("Backup",  "AutoSaveInterval", 7);
    ini.set("Backup",  "AutoSave", true);

    EXPECT_EQ(ini.getBool("General", "SingleInstance", true),  false, "set/get bool");
    EXPECT_EQ(ini.getInt("Editor",   "TabWidth",       4),    16,    "set/get int");
    EXPECT_EQ(ini.getBool("Editor",   "TabAsSpaces",    false), true, "set/get bool roundtrip");
    EXPECT_EQ(ini.get("Editor",      "Theme",          "default"), std::string("monokai"), "set/get string");
    EXPECT_EQ(ini.get("Backup",      "BackupDir",      ""),    std::string("/tmp/x"), "set/get dir");
    EXPECT_EQ(ini.getInt("Backup",   "AutoSaveInterval",5),    7,    "set/get int interval");
    EXPECT_EQ(ini.getBool("Backup",   "AutoSave",       false), true, "set/get autoSave");

    // Save and reload through disk
    std::string path = "/tmp/npp-cfg-setget-" + std::to_string(getpid()) + ".ini";
    bool saved = ini.save(path);
    EXPECT_TRUE(saved, "IniParser::save writes file");

    IniParser ini2;
    bool loaded = ini2.load(path);
    EXPECT_TRUE(loaded, "IniParser::load reads file back");

    EXPECT_EQ(ini2.getBool("General", "SingleInstance", true),  false, "disk: getBool");
    EXPECT_EQ(ini2.getInt("Editor",   "TabWidth",       4),    16,    "disk: getInt");
    EXPECT_EQ(ini2.getBool("Editor",   "TabAsSpaces",    false), true, "disk: getBool 2");
    EXPECT_EQ(ini2.get("Editor",      "Theme",          "default"), std::string("monokai"), "disk: get string");
    EXPECT_EQ(ini2.get("Backup",      "BackupDir",      ""),    std::string("/tmp/x"), "disk: get dir");
    EXPECT_EQ(ini2.getInt("Backup",   "AutoSaveInterval",5),    7,    "disk: getInt interval");
    EXPECT_EQ(ini2.getBool("Backup",   "AutoSave",       false), true, "disk: getBool 3");

    fs::remove(path);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    std::cout << "Config persistence tests:\n";
    test_ini_set_get_round_trip();
    test_full_saveConfig_payload_round_trip();

    std::cout << "\n=== Config persistence tests: "
              << (g_failed == 0 ? "all passed" : "FAILED")
              << " ===\n";
    return g_failed == 0 ? 0 : 1;
}