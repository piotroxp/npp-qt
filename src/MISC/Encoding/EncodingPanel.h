// MISC/Encoding/EncodingPanel.h - Qt6 port of encoding selection panel
#pragma once

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class EncodingPanel : public QDialog
{
    Q_OBJECT
public:
    explicit EncodingPanel(QWidget* parent = nullptr);
    ~EncodingPanel() = default;
    
    int getSelectedEncoding() const { return _selectedEncoding; }
    void setCurrentEncoding(int codepage);
    
signals:
    void encodingSelected(int codepage);

private slots:
    void onEncodingChanged(int index);
    void onOkClicked();
    void onCancelClicked();
    
private:
    void populateEncodings();
    void populateEolModes();
    
    QComboBox* _encodingCombo;
    QComboBox* _eolCombo;
    QLabel* _previewLabel;
    int _selectedEncoding;
};

// EOL type selection
enum class EolMode {
    Windows,  // CRLF
    Unix,     // LF
    Mac       // CR
};