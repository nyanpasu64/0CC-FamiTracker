/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2018 HertzDevil
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/


#pragma once

#include "stdafx.h"
#include "APU/Types_fwd.h"
#include <string_view>

class CRegisterDisplay {		// // // TODO: move to its own thread
public:
	CRegisterDisplay(CDC &dc, COLORREF bgColor);
	void Draw();

private:
	void DrawHeader(const CStringA &text);
	void DrawReg(const CStringA &header, int count);
	void DrawText_(int xOffs, const CStringA &text);
	template <typename F>
	void GetRegs(sound_chip_t Chip, F f, int Count);

private:
	CDC &dc_;
	COLORREF bgColor_;

	static const int LINE_HEIGHT = 13;		// // //
	int x = 30;
	int y = 30 - LINE_HEIGHT * 2;
	int line = -1;

	unsigned char reg[8] = { };
	unsigned char update[8] = { };
};
