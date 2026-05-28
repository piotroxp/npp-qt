#include "ScintillaComponent/Buffer.h"

void FileManager::init(Notepad_plus* pNotepadPlus, ScintillaEditView* pscratchTilla)
{
    Q_UNUSED(pNotepadPlus);
    Q_UNUSED(pscratchTilla);
}

BufferID FileManager::loadFile(const wchar_t* filename, Document doc, int encoding)
{
    Q_UNUSED(filename);
    Q_UNUSED(doc);
    Q_UNUSED(encoding);
    return BUFFER_INVALID;
}

BufferID FileManager::newEmptyDocument()
{
    return BUFFER_INVALID;
}
