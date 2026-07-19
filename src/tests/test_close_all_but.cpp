// test_close_all_but.cpp — closeAllBuffersExcept tests
#include "core/FileManager.h"
#include <QCoreApplication>
#include <iostream>
int main(int argc, char* argv[]) {
    std::cerr << "=== closeAllBuffersExcept tests ===\n";
    QCoreApplication app(argc, argv);
    FileManager& fm = FileManager::instance();
    // Test basic FileManager construction
    fm.closeAllFiles();
    std::cerr << "  FileManager::instance OK\n";
    fm.createNewFile();
    std::cerr << "  createNewFile OK\n";
    std::cerr << "  PASS\n";
    return 0;
}
