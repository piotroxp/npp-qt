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


// verifySignedfile.h : Verification of an Authenticode signed DLL
#pragma once

#include <string>
#include <vector>

enum SecurityMode { sm_certif = 0, sm_sha256 = 1 };
enum NppModule { nm_gup = 1, nm_pluginList = 2 };

class SecurityGuard final
{
public:
	SecurityGuard();

	bool checkModule(const std::wstring& filePath, NppModule module2check);

	std::wstring signer_display_name() { return _signer_display_name; }
	std::wstring signer_subject() { return _signer_subject; }
	std::wstring signer_key_id() { return _signer_key_id; }

private:
	// SHA256
	static SecurityMode _securityMode;
	std::vector<std::wstring> _gupSha256;
	std::vector<std::wstring> _pluginListSha256;

	bool checkSha256(const std::wstring& filePath, NppModule module2check);

	// Code signing certificate
	std::wstring _signer_display_name = L"NOTEPAD++";
	std::wstring _signer_subject = L"C=FR, S=Île-de-France, L=Paris, O=\"NOTEPAD++\", CN=\"NOTEPAD++\", E=don.h@free.fr";
	std::wstring _signer_key_id = L"CC0D94922CDA3A18A7D286138525AF9C3942E9E7"; //=> Should be UPPERCASE

	bool _doCheckRevocation = false;
	bool _doCheckChainOfTrust = false;

	bool verifySignedBinary(const std::wstring& filepath);
};
