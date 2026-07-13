// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <utility>
#include <random>
#include <cstring>    // for _wcsicmp on Windows; on Linux use Qt wrappers below
#include <QLocale>
#include <QCollator>

// _wcsicmp is a Microsoft extension; on non-Windows platforms, use QString-based comparison
#ifndef _WIN32
inline int wcsicmp_portable(const wchar_t* a, const wchar_t* b) {
    if (!a || !b) return (a ? 1 : 0) - (b ? 1 : 0);
    QString sa = QString::fromWCharArray(a);
    QString sb = QString::fromWCharArray(b);
    return QString::compare(sa, sb, Qt::CaseInsensitive);
}
#define _wcsicmp wcsicmp_portable
#endif

// Base interface for line sorting.
class ISorter
{
private:
	bool _isDescending = true;
	size_t _fromColumn = 0;
	size_t _toColumn = 0;

protected:
	bool isDescending() const {
		return _isDescending;
	}

	std::wstring getSortKey(const std::wstring& input) {
		if (isSortingSpecificColumns())
		{
			if (input.length() < _fromColumn)
			{
				return L"";
			}
			else if (_fromColumn == _toColumn)
			{
				return input.substr(_fromColumn);
			}
			else
			{
				return input.substr(_fromColumn, _toColumn - _fromColumn);
			}
		}
		else
		{
			return input;
		}
	}

	bool isSortingSpecificColumns() {
		return _toColumn != 0;
	}

public:
	ISorter(bool isDescending, size_t fromColumn, size_t toColumn) : _isDescending(isDescending), _fromColumn(fromColumn), _toColumn(toColumn) {
		assert(_fromColumn <= _toColumn);
	}
	virtual ~ISorter() {}
	virtual void sort(std::vector<std::wstring>& lines) = 0;
};

// Implementation of lexicographic sorting of lines.
class LexicographicSorter : public ISorter
{
public:
	LexicographicSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) {}

	void sort(std::vector<std::wstring>& lines) override {
		if (isSortingSpecificColumns())
		{
			std::stable_sort(lines.begin(), lines.end(), [this](const std::wstring& a, const std::wstring& b)
			{
				if (isDescending())
				{
					return getSortKey(a).compare(getSortKey(b)) > 0;
				}
				else
				{
					return getSortKey(a).compare(getSortKey(b)) < 0;
				}
			});
		}
		else
		{
			std::sort(lines.begin(), lines.end(), [this](const std::wstring& a, const std::wstring& b)
			{
				if (isDescending())
				{
					return a.compare(b) > 0;
				}
				else
				{
					return a.compare(b) < 0;
				}
			});
		}
	}
};

// Implementation of lexicographic sorting of lines, ignoring character casing
class LexicographicCaseInsensitiveSorter : public ISorter
{
public:
	LexicographicCaseInsensitiveSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) {}

	void sort(std::vector<std::wstring>& lines) override {
		if (isSortingSpecificColumns())
		{
			std::stable_sort(lines.begin(), lines.end(), [this](const std::wstring& a, const std::wstring& b)
				{
					if (isDescending())
					{
						return _wcsicmp(getSortKey(a).c_str(), getSortKey(b).c_str()) > 0;
					}
					else
					{
						return _wcsicmp(getSortKey(a).c_str(), getSortKey(b).c_str()) < 0;
					}
				});
		}
		else
		{
			std::sort(lines.begin(), lines.end(), [this](const std::wstring& a, const std::wstring& b)
				{
					if (isDescending())
					{
						return _wcsicmp(a.c_str(), b.c_str()) > 0;
					}
					else
					{
						return _wcsicmp(a.c_str(), b.c_str()) < 0;
					}
				});
		}
	}
};

class IntegerSorter : public ISorter
{
public:
	IntegerSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) {}

	void sort(std::vector<std::wstring>& lines) override {
		if (isSortingSpecificColumns())
		{
			std::stable_sort(lines.begin(), lines.end(), [this](std::wstring aIn, std::wstring bIn)
			{
				std::wstring a = getSortKey(aIn);
				std::wstring b = getSortKey(bIn);

				long long compareResult = 0;
				size_t aNumIndex = 0;
				size_t bNumIndex = 0;
				while (compareResult == 0)
				{
					if (aNumIndex >= a.length() || bNumIndex >= b.length())
					{
						compareResult = a.compare(std::min<size_t>(aNumIndex, a.length()), std::wstring::npos, b, std::min<size_t>(bNumIndex, b.length()), std::wstring::npos);
						break;
					}

					bool aChunkIsNum = a[aNumIndex] >= L'0' && a[aNumIndex] <= L'9';
					bool bChunkIsNum = b[bNumIndex] >= L'0' && b[bNumIndex] <= L'9';

					int aNumSign = 1;
					if (!aChunkIsNum && (aNumIndex + 1) < a.length())
					{
						aChunkIsNum = (a[aNumIndex] == L'-' && (a[aNumIndex + 1] >= L'0' && a[aNumIndex + 1] <= L'9'));
						aNumSign = -1;
					}

					int bNumSign = 1;
					if (!bChunkIsNum && (bNumIndex + 1) < b.length())
					{
						bChunkIsNum = (b[bNumIndex] == L'-' && (b[bNumIndex + 1] >= L'0' && b[bNumIndex + 1] <= L'9'));
						bNumSign = -1;
					}

					if (aChunkIsNum != bChunkIsNum)
					{
						compareResult = a[aNumIndex] - b[bNumIndex];
						aNumIndex++;
						bNumIndex++;
					}
					else if (aChunkIsNum)
					{
						if (aNumSign != bNumSign)
						{
							compareResult = (aNumSign == 1) ? 1 : -1;
						}
						else
						{
							if (aNumSign == -1)
							{
								aNumIndex++;
								bNumIndex++;
							}

							size_t aNumEnd = a.find_first_not_of(L"1234567890", aNumIndex);
							if (aNumEnd == std::wstring::npos)
							{
								aNumEnd = a.length();
							}

							size_t bNumEnd = b.find_first_not_of(L"1234567890", bNumIndex);
							if (bNumEnd == std::wstring::npos)
							{
								bNumEnd = b.length();
							}

							int aZeroNum = 0;
							while (aNumIndex < a.length() && a[aNumIndex] == '0')
							{
								aZeroNum++;
								aNumIndex++;
							}

							int bZeroNum = 0;
							while (bNumIndex < b.length() && b[bNumIndex] == '0')
							{
								bZeroNum++;
								bNumIndex++;
							}

							size_t aNumLength = aNumEnd - aNumIndex;
							size_t bNumLength = bNumEnd - bNumIndex;

							if (aNumLength > bNumLength)
							{
								compareResult = 1 * aNumSign;
							}
							else if (aNumLength < bNumLength)
							{
								compareResult = -1 * aNumSign;
							}
							else
							{
								while (compareResult == 0
									&& aNumIndex < a.length()
									&& (a[aNumIndex] >= L'0' && a[aNumIndex] <= L'9')
									&& bNumIndex < b.length()
									&& (b[bNumIndex] >= L'0' && b[bNumIndex] <= L'9'))
								{
									compareResult = (a[aNumIndex] - b[bNumIndex]) * aNumSign;
									aNumIndex++;
									bNumIndex++;
								}

								if (compareResult == 0)
								{
									compareResult = bZeroNum - aZeroNum;
								}
							}
						}
					}
					else
					{
						if (a[aNumIndex] == L'-')
						{
							aNumIndex++;
						}

						if (b[bNumIndex] == L'-')
						{
							bNumIndex++;
						}

						size_t aChunkEnd = a.find_first_of(L"1234567890-", aNumIndex);
						size_t bChunkEnd = b.find_first_of(L"1234567890-", bNumIndex);
						compareResult = a.compare(aNumIndex, aChunkEnd - aNumIndex, b, bNumIndex, bChunkEnd - bNumIndex);
						aNumIndex = aChunkEnd;
						bNumIndex = bChunkEnd;
					}
				}

				if (isDescending())
				{
					return compareResult > 0;
				}
				else
				{
					return compareResult < 0;
				}
			});
		}
		else
		{
			std::sort(lines.begin(), lines.end(), [this](std::wstring aIn, std::wstring bIn)
			{
				std::wstring a = aIn;
				std::wstring b = bIn;

				long long compareResult = 0;
				size_t aNumIndex = 0;
				size_t bNumIndex = 0;
				while (compareResult == 0)
				{
					if (aNumIndex >= a.length() || bNumIndex >= b.length())
					{
						compareResult = a.compare(std::min<size_t>(aNumIndex, a.length()), std::wstring::npos, b, std::min<size_t>(bNumIndex, b.length()), std::wstring::npos);
						break;
					}

					bool aChunkIsNum = a[aNumIndex] >= L'0' && a[aNumIndex] <= L'9';
					bool bChunkIsNum = b[bNumIndex] >= L'0' && b[bNumIndex] <= L'9';

					int aNumSign = 1;
					if (!aChunkIsNum && (aNumIndex + 1) < a.length())
					{
						aChunkIsNum = (a[aNumIndex] == L'-' && (a[aNumIndex + 1] >= L'0' && a[aNumIndex + 1] <= L'9'));
						aNumSign = -1;
					}

					int bNumSign = 1;
					if (!bChunkIsNum && (bNumIndex + 1) < b.length())
					{
						bChunkIsNum = (b[bNumIndex] == L'-' && (b[bNumIndex + 1] >= L'0' && b[bNumIndex + 1] <= L'9'));
						bNumSign = -1;
					}

					if (aChunkIsNum != bChunkIsNum)
					{
						compareResult = a[aNumIndex] - b[bNumIndex];
						aNumIndex++;
						bNumIndex++;
					}
					else if (aChunkIsNum)
					{
						if (aNumSign != bNumSign)
						{
							compareResult = (aNumSign == 1) ? 1 : -1;
						}
						else
						{
							if (aNumSign == -1)
							{
								aNumIndex++;
								bNumIndex++;
							}

							size_t aNumEnd = a.find_first_not_of(L"1234567890", aNumIndex);
							if (aNumEnd == std::wstring::npos)
							{
								aNumEnd = a.length();
							}

							size_t bNumEnd = b.find_first_not_of(L"1234567890", bNumIndex);
							if (bNumEnd == std::wstring::npos)
							{
								bNumEnd = b.length();
							}

							int aZeroNum = 0;
							while (aNumIndex < a.length() && a[aNumIndex] == '0')
							{
								aZeroNum++;
								aNumIndex++;
							}

							int bZeroNum = 0;
							while (bNumIndex < b.length() && b[bNumIndex] == '0')
							{
								bZeroNum++;
								bNumIndex++;
							}

							size_t aNumLength = aNumEnd - aNumIndex;
							size_t bNumLength = bNumEnd - bNumIndex;

							if (aNumLength > bNumLength)
							{
								compareResult = 1 * aNumSign;
							}
							else if (aNumLength < bNumLength)
							{
								compareResult = -1 * aNumSign;
							}
							else
							{
								while (compareResult == 0
									&& aNumIndex < a.length()
									&& (a[aNumIndex] >= L'0' && a[aNumIndex] <= L'9')
									&& bNumIndex < b.length()
									&& (b[bNumIndex] >= L'0' && b[bNumIndex] <= L'9'))
								{
									compareResult = (a[aNumIndex] - b[bNumIndex]) * aNumSign;
									aNumIndex++;
									bNumIndex++;
								}

								if (compareResult == 0)
								{
									compareResult = bZeroNum - aZeroNum;
								}
							}
						}
					}
					else
					{
						if (a[aNumIndex] == L'-')
						{
							aNumIndex++;
						}

						if (b[bNumIndex] == L'-')
						{
							bNumIndex++;
						}

						size_t aChunkEnd = a.find_first_of(L"1234567890-", aNumIndex);
						size_t bChunkEnd = b.find_first_of(L"1234567890-", bNumIndex);
						compareResult = a.compare(aNumIndex, aChunkEnd - aNumIndex, b, bNumIndex, bChunkEnd - bNumIndex);
						aNumIndex = aChunkEnd;
						bNumIndex = bChunkEnd;
					}
				}

				if (isDescending())
				{
					return compareResult > 0;
				}
				else
				{
					return compareResult < 0;
				}
			});
		}
	}
};

// Convert each line to a number and then sort.
template<typename T_Num>
class NumericSorter : public ISorter
{
public:
	NumericSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn)
	{
#ifdef _WIN32
#ifndef __MINGW32__
		_usLocale = ::_create_locale(LC_NUMERIC, "en-US");
#else
		_usLocale = NULL;
#endif
#else
		_usLocale = QLocale(QLocale::English, QLocale::UnitedStates);
#endif
	}

	~NumericSorter() override
	{
#ifdef _WIN32
#ifndef __MINGW32__
		::_free_locale(_usLocale);
#endif
#endif
	}

	void sort(std::vector<std::wstring>& lines) override {
		std::vector<std::pair<size_t, T_Num>> nonEmptyInputAsNumbers;
		std::vector<std::wstring> empties;
		nonEmptyInputAsNumbers.reserve(lines.size());
		for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex)
		{
			const std::wstring originalLine = lines[lineIndex];
			const std::wstring preparedLine = prepareStringForConversion(originalLine);
			if (considerStringEmpty(preparedLine))
			{
				empties.push_back(originalLine);
			}
			else
			{
				try
				{
					nonEmptyInputAsNumbers.push_back(std::make_pair(lineIndex, convertStringToNumber(preparedLine)));
				}
				catch (...)
				{
					throw lineIndex;
				}
			}
		}

		assert(nonEmptyInputAsNumbers.size() + empties.size() == lines.size());
		const bool descending = isDescending();
		std::stable_sort(nonEmptyInputAsNumbers.begin(), nonEmptyInputAsNumbers.end(), [descending](std::pair<size_t, T_Num> a, std::pair<size_t, T_Num> b)
			{
				if (descending)
				{
					return a.second > b.second;
				}
				else
				{
					return a.second < b.second;
				}
			});

		std::vector<std::wstring> output;
		output.reserve(lines.size());
		if (!isDescending())
		{
			output.insert(output.end(), empties.begin(), empties.end());
		}

		for (auto it = nonEmptyInputAsNumbers.begin(); it != nonEmptyInputAsNumbers.end(); ++it)
		{
			output.push_back(lines[it->first]);
		}

		if (isDescending())
		{
			output.insert(output.end(), empties.begin(), empties.end());
		}

		assert(output.size() == lines.size());
		lines = output;
	}

protected:
	bool considerStringEmpty(const std::wstring& input) {
		return input.find_first_not_of(L" \t\r\n") == std::string::npos;
	}

	virtual std::wstring prepareStringForConversion(const std::wstring& input) = 0;
	virtual T_Num convertStringToNumber(const std::wstring& input) = 0;

#ifdef _WIN32
	_locale_t _usLocale;
#else
	QLocale _usLocale;
#endif
};

// Converts lines to double before sorting (assumes decimal comma).
class DecimalCommaSorter : public NumericSorter<double>
{
public:
	DecimalCommaSorter(bool isDescending, size_t fromColumn, size_t toColumn) : NumericSorter<double>(isDescending, fromColumn, toColumn) {}

protected:
	std::wstring prepareStringForConversion(const std::wstring& input) override {
		std::wstring admissablePart = stringTakeWhileAdmissable(getSortKey(input), L" \t\r\n0123456789,-");
		return stringReplace(admissablePart, L",", L".");
	}

	double convertStringToNumber(const std::wstring& input) override {
		return stodLocale(input, _usLocale);
	}
};

// Converts lines to double before sorting (assumes decimal dot).
class DecimalDotSorter : public NumericSorter<double>
{
public:
	DecimalDotSorter(bool isDescending, size_t fromColumn, size_t toColumn) : NumericSorter<double>(isDescending, fromColumn, toColumn) {}

protected:
	std::wstring prepareStringForConversion(const std::wstring& input) override {
		return stringTakeWhileAdmissable(getSortKey(input), L" \t\r\n0123456789.-");
	}

	double convertStringToNumber(const std::wstring& input) override {
		return stodLocale(input, _usLocale);
	}
};

class ReverseSorter : public ISorter
{
public:
	ReverseSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) {}

	void sort(std::vector<std::wstring>& lines) override {
		std::reverse(lines.begin(), lines.end());
	}
};

class RandomSorter : public ISorter
{
public:
	unsigned seed;

	RandomSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) {
		seed = static_cast<unsigned>(time(nullptr));
	}

	void sort(std::vector<std::wstring>& lines) override {
		std::shuffle(lines.begin(), lines.end(), std::default_random_engine(seed));
	}
};

// Implementation of length wise sorting of lines.
class LineLengthSorter : public ISorter
{
public:
	LineLengthSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) {}

	void sort(std::vector<std::wstring>& lines) override {
		if (isSortingSpecificColumns())
		{
			std::stable_sort(lines.begin(), lines.end(), [this](std::wstring a, std::wstring b)
				{
					if (isDescending())
					{
						return getSortKey(a).length() > getSortKey(b).length();
					}
					else
					{
						return getSortKey(a).length() < getSortKey(b).length();
					}
				});
		}
		else
		{
			std::sort(lines.begin(), lines.end(), [this](std::wstring a, std::wstring b)
				{
					if (isDescending())
					{
						return a.length() > b.length();
					}
					else
					{
						return a.length() < b.length();
					}
				});
		}
	}
};

// Helper: take characters from input while they are in admissable set
inline std::wstring stringTakeWhileAdmissable(const std::wstring& input, const std::wstring& admissable)
{
	std::wstring result;
	for (wchar_t ch : input)
	{
		if (admissable.find(ch) != std::wstring::npos)
			result += ch;
		else
			break;
	}
	return result;
}

// Helper: replace all occurrences of a substring
inline std::wstring stringReplace(const std::wstring& input, const std::wstring& from, const std::wstring& to)
{
	std::wstring result = input;
	size_t pos = 0;
	while ((pos = result.find(from, pos)) != std::wstring::npos)
	{
		result.replace(pos, from.length(), to);
		pos += to.length();
	}
	return result;
}

// Helper: convert string to double using fixed locale (en-US)
template<typename T>
T stodLocale(const std::wstring& input, const QLocale& locale)
{
	return locale.toDouble(QString::fromStdWString(input));
}
