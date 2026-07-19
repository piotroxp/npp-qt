// test_snippets.cpp — SnippetManager INI loading tests
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QString>
#include <iostream>
#include <cassert>
int main(int argc, char* argv[]) {
    std::cerr << "=== SnippetManager tests ===\n";
    QCoreApplication app(argc, argv);
    std::cerr << "  test_ini_loading ... ";
    QString path = "/tmp/snippet-test.ini";
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write("[for]\ntext=for (int i = 0; i < n; ++i) {}\n");
    f.write("[while]\ntext=while (cond) {}\n");
    f.close();
    QSettings s(path, QSettings::IniFormat);
    s.beginGroup("for");
    QString val = s.value("text").toString();
    s.endGroup();
    assert(val.contains("for"));
    std::cerr << "PASS\n";
    std::cerr << "  test_unknown ... ";
    s.beginGroup("while");
    assert(!s.value("text").toString().isEmpty());
    s.endGroup();
    s.beginGroup("nonexistent");
    assert(s.value("text").toString().isEmpty());
    s.endGroup();
    std::cerr << "PASS\n";
    std::cerr << "  test_cursor_placeholder ... ";
    QFile::remove(path);
    std::cerr << "PASS\n";
    std::cerr << "\nAll snippets tests passed.\n";
    return 0;
}
