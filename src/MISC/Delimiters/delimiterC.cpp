// MISC/Delimiters/delimiterC.cpp - Qt6 port of comment/section delimiters
#include "delimiterC.h"

DelimiterC::DelimiterC()
{
    initializeBuiltInDelimiters();
}

DelimiterC& DelimiterC::getInstance()
{
    static DelimiterC instance;
    return instance;
}

DelimiterSet DelimiterC::cppDelimiters()
{
    return DelimiterSet("//", "/*", "*/", "\"");
}

DelimiterSet DelimiterC::pythonDelimiters()
{
    return DelimiterSet("#", "\"\"\"", "\"\"\"", "\"");
}

DelimiterSet DelimiterC::htmlDelimiters()
{
    return DelimiterSet("", "<!--", "-->", "\"");
}

DelimiterSet DelimiterC::xmlDelimiters()
{
    return DelimiterSet("", "<!--", "-->", "\"");
}

DelimiterSet DelimiterC::sqlDelimiters()
{
    return DelimiterSet("--", "/*", "*/", "'");
}

DelimiterSet DelimiterC::bashDelimiters()
{
    return DelimiterSet("#", "", "", "\"");
}

DelimiterSet DelimiterC::pascalDelimiters()
{
    return DelimiterSet("//", "(*", "*)", "'");
}

DelimiterSet DelimiterC::perlDelimiters()
{
    return DelimiterSet("#", "=pod", "=cut", "\"");
}

DelimiterSet DelimiterC::phpDelimiters()
{
    return DelimiterSet("//", "/*", "*/", "\"");
}

void DelimiterC::initializeBuiltInDelimiters()
{
    _delimiterSets["C"] = cppDelimiters();
    _delimiterSets["C++"] = cppDelimiters();
    _delimiterSets["C#"] = cppDelimiters();
    _delimiterSets["Java"] = cppDelimiters();
    _delimiterSets["JavaScript"] = cppDelimiters();
    _delimiterSets["PHP"] = phpDelimiters();
    _delimiterSets["Python"] = pythonDelimiters();
    _delimiterSets["Ruby"] = DelimiterSet("#", "=begin", "=end", "\"");
    _delimiterSets["HTML"] = htmlDelimiters();
    _delimiterSets["XML"] = xmlDelimiters();
    _delimiterSets["SQL"] = sqlDelimiters();
    _delimiterSets["Bash"] = bashDelimiters();
    _delimiterSets["Shell"] = bashDelimiters();
    _delimiterSets["Pascal"] = pascalDelimiters();
    _delimiterSets["Perl"] = perlDelimiters();
    _delimiterSets["Fortran"] = DelimiterSet("C", "!", "", "'");
    _delimiterSets["Makefile"] = DelimiterSet("#", "", "", "");
}

DelimiterSet DelimiterC::getDelimiters(const QString& language) const
{
    auto it = _delimiterSets.find(language);
    if (it != _delimiterSets.end())
        return it.value();
    
    // Return C-style delimiters as default
    return cppDelimiters();
}

void DelimiterC::setDelimiters(const QString& language, const DelimiterSet& delimiters)
{
    _delimiterSets[language] = delimiters;
}

bool DelimiterC::isCommentLine(const QString& lang, const QString& line) const
{
    DelimiterSet delim = getDelimiters(lang);
    if (delim.commentLine.isEmpty())
        return false;
    return line.trimmed().startsWith(delim.commentLine);
}

bool DelimiterC::isStringDelimiter(const QString& lang, QChar c) const
{
    DelimiterSet delim = getDelimiters(lang);
    return delim.stringChar.contains(c);
}