// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

#pragma once

#include "WinControls/StaticDialog/StaticDialog.h"

enum hashType {hash_md5 = 16, hash_sha1 = 20, hash_sha256 = 32, hash_sha512 = 64};

#define HASH_MAX_LENGTH hash_sha512
#define HASH_STR_MAX_LENGTH (hash_sha512 * 2 + 1)

class HashFromFilesDlg : public StaticDialog
{
public :
	HashFromFilesDlg() = default;

	void doDialog(bool isRTL = false);
	void destroy() override;
	void setHashType(hashType hashType2set);

protected :
	intptr_t CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	hashType _ht = hash_md5;

private :
	HFONT _hFont = nullptr;
};

class HashFromTextDlg : public StaticDialog
{
public :
	HashFromTextDlg() = default;

	void doDialog(bool isRTL = false);
	void destroy() override;
	void generateHash();
	void generateHashPerLine();
	void setHashType(hashType hashType2set);

protected :
	intptr_t CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	hashType _ht = hash_md5;

private :
	HFONT _hFont = nullptr;
};
