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

#include "PatternClipData.h"
#include "ft0cc/doc/pattern_note.hpp"
#include "Assertion.h"		// // //
#include "ArrayStream.h"		// // //

CPatternClipData::CPatternClipData(unsigned Channels, unsigned Rows) :
	ClipInfo({Channels, Rows}),		// // //
	pPattern(std::make_unique<value_type[]>(Channels * Rows))
{
}

std::size_t CPatternClipData::GetAllocSize() const
{
	return sizeof(stClipInfo) + ClipInfo.GetSize() * sizeof(value_type);
}

bool CPatternClipData::ContainsData() const {		// // //
	return pPattern != nullptr;
}

bool CPatternClipData::ToBytes(array_view<std::byte> Buf) const {		// // //
	try {
		if (Buf.size() >= GetAllocSize()) {
			CArrayStream stream {Buf};

			stream.WriteInt<std::uint32_t>(ClipInfo.Channels);
			stream.WriteInt<std::uint32_t>(ClipInfo.Rows);
			stream.WriteInt<std::uint32_t>(value_cast(ClipInfo.StartColumn));
			stream.WriteInt<std::uint32_t>(value_cast(ClipInfo.EndColumn));
			stream.WriteInt<std::uint32_t>(ClipInfo.OleInfo.ChanOffset);
			stream.WriteInt<std::uint32_t>(ClipInfo.OleInfo.RowOffset);

			const std::size_t Size = ClipInfo.GetSize();
			for (std::size_t i = 0; i < Size; ++i) {
				const auto &note = pPattern[i];
				stream.WriteInt<std::uint8_t>(value_cast(note.note()));
				stream.WriteInt<std::uint8_t>(note.oct());
				stream.WriteInt<std::uint8_t>(note.vol());
				stream.WriteInt<std::uint8_t>(note.inst());
				for (const auto &cmd : note.fx_cmds())
					stream.WriteInt<std::uint8_t>(value_cast(cmd.fx));
				for (const auto &cmd : note.fx_cmds())
					stream.WriteInt<std::uint8_t>(cmd.param);
			}
			return true;
		}
		return false;
	}
	catch (CBinaryIOException &) {
		return false;
	}
}

bool CPatternClipData::FromBytes(array_view<const std::byte> Buf) {		// // //
	try {
		if (Buf.size() >= sizeof(stClipInfo)) {
			CConstArrayStream stream {Buf};

			stClipInfo info = { };
			info.Channels = stream.ReadInt<std::uint32_t>();
			info.Rows = stream.ReadInt<std::uint32_t>();
			info.StartColumn = enum_cast<column_t>(stream.ReadInt<std::uint32_t>());
			info.EndColumn = enum_cast<column_t>(stream.ReadInt<std::uint32_t>());
			info.OleInfo.ChanOffset = stream.ReadInt<std::uint32_t>();
			info.OleInfo.RowOffset = stream.ReadInt<std::uint32_t>();

			const std::size_t Size = info.GetSize();
			if (Buf.size() >= sizeof(stClipInfo) + Size * sizeof(value_type)) {
				pPattern = std::make_unique<value_type[]>(Size);		// // //
				ClipInfo = info;
				for (std::size_t i = 0; i < Size; ++i) {
					auto &note = pPattern[i];
					note.set_note(enum_cast<ft0cc::doc::pitch>(stream.ReadInt<std::uint8_t>()));
					note.set_oct(stream.ReadInt<std::uint8_t>());
					note.set_vol(stream.ReadInt<std::uint8_t>());
					note.set_inst(stream.ReadInt<std::uint8_t>());
					for (auto &cmd : note.fx_cmds())
						cmd.fx = enum_cast<ft0cc::doc::effect_type>(stream.ReadInt<std::uint8_t>());
					for (auto &cmd : note.fx_cmds())
						cmd.param = stream.ReadInt<std::uint8_t>();
				}
				return true;
			}
		}
		return false;
	}
	catch (CBinaryIOException &) {
		return false;
	}
}

ft0cc::doc::pattern_note *CPatternClipData::GetPattern(unsigned Channel, unsigned Row)
{
	Assert(Channel < ClipInfo.Channels);
	Assert(Row < ClipInfo.Rows);

	return &pPattern[Channel * ClipInfo.Rows + Row];
}

const ft0cc::doc::pattern_note *CPatternClipData::GetPattern(unsigned Channel, unsigned Row) const
{
	Assert(Channel < ClipInfo.Channels);
	Assert(Row < ClipInfo.Rows);

	return &pPattern[Channel * ClipInfo.Rows + Row];
}
