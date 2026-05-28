// This file is part of Notepad++ project
// Copyright (C)2024 Don HO <don.h@free.fr>

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
#include "MISC/Common/WindowsCompat.h"

class Window
{
public:
	//! \name Constructors & Destructor
	//@{
	Window() = default;
	Window(const Window&) = delete;
	virtual ~Window() = default;
	//@}


	virtual void init(HINSTANCE hInst, HWND parent) {
		_hInst = hInst;
		_hParent = parent;
	}

	virtual void destroy() = 0;

	virtual void display(bool toShow = true) const {
		::ShowWindow(_hSelf, toShow ? SW_SHOW : SW_HIDE);
	}


	virtual void reSizeTo(RECT & rc) { // should NEVER be const !!!
		_hSelf->setGeometry(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		redraw();
	}


	virtual void reSizeToWH(RECT& rc) { // should NEVER be const !!!
		_hSelf->setGeometry(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		redraw();
	}


	virtual void redraw(bool forceUpdate = false) const {
		_hSelf->update();
		if (forceUpdate)
			_hSelf->repaint();
	}


    virtual void getClientRect(RECT & rc) const {
		{ QRect r = _hSelf->rect(); rc.left=r.left(); rc.top=r.top(); rc.right=r.right(); rc.bottom=r.bottom(); }
	}

	virtual void getWindowRect(RECT & rc) const {
		{ QRect r = _hSelf->geometry(); rc.left=r.x(); rc.top=r.y(); rc.right=r.x()+r.width(); rc.bottom=r.y()+r.height(); }
	}

	virtual int getWidth() const {
		RECT rc;
		{ QRect r = _hSelf->rect(); rc.left=r.left(); rc.top=r.top(); rc.right=r.right(); rc.bottom=r.bottom(); }
		return (rc.right - rc.left);
	}

	virtual int getHeight() const {
		RECT rc;
		{ QRect r = _hSelf->rect(); rc.left=r.left(); rc.top=r.top(); rc.right=r.right(); rc.bottom=r.bottom(); }
		if (_hSelf->isVisible())
			return (rc.bottom - rc.top);
		return 0;
	}

	virtual bool isVisible() const
	{
    	return (_hSelf->isVisible());
	}

	HWND getHSelf() const {
		return _hSelf;
	}

	HWND getHParent() const {
		return _hParent;
	}

	void grabFocus() const {
		::SetFocus(_hSelf);
	}

    HINSTANCE getHinst() const {
		return _hInst;
	}


	Window& operator = (const Window&) = delete;


protected:
	HINSTANCE _hInst = NULL;
	HWND _hParent = NULL;
	HWND _hSelf = NULL;
};