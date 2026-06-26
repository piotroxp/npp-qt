// Ported to Qt6
#pragma once
#include <QQueue>
#include <QObject>

class QMenu;  // forward — full type only needed in .cpp

class LastRecentFileList : public QObject {
    Q_OBJECT
public:
    explicit LastRecentFileList(QObject* parent = nullptr);
    void initMenu(QMenu* hMenu, int idBase, int posBase, bool doSubMenu = false);
    void switchMode();
    void updateMenu();
    void add(const QString& fn);
    void remove(const QString& fn);
    void remove(size_t index);
    void clear();
    int getSize() const { return _size; }
    static int getMaxNbLRF() { return 30; }
    int getUserMaxNbLRF() const { return _userMax; }
    QString getItem(int id) const;
    QString getIndex(int index) const;
    QString getFirstItem() const;
    void setUserMaxNbLRF(int size);
    void saveLRFL();
    void loadLRFL();
    void setLock(bool lock) { _locked = lock; }
    void setLangEncoding(int enc) { _nativeLangEncoding = enc; }
    bool isSubMenuMode() const { return _hParentMenu != nullptr; }
signals:
    void itemSelected(const QString& filePath);
private:
    struct RecentItem {
        int _id = 0;
        QString _name;
        explicit RecentItem(const QString& name) : _name(name) {}
    };
    QQueue<RecentItem> _lrfl;
    int _userMax = 30;
    int _size = 0;
    int _nativeLangEncoding = -1;
    QMenu* _hParentMenu = nullptr;
    QMenu* _hMenu = nullptr;
    int _posBase = -1;
    int _idBase = -1;
    QVector<bool> _idFreeArray;
    bool _hasSeparators = false;
    bool _locked = false;
    int find(const QString& fn) const;
    int popFirstAvailableID();
    void setAvailable(int id);
};
