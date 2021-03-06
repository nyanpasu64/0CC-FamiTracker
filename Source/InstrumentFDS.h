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

#include "SeqInstrument.h"		// // //
#include <unordered_map>		// // //
#include <memory>
#include <array>		// // //
#include "ft0cc/cpputil/array_view.hpp"		// // //

class CInstrumentFDS : public CSeqInstrument {
public:
	CInstrumentFDS();
	std::unique_ptr<CInstrument> Clone() const override;
	bool	CanRelease() const override;

public:
	unsigned char GetSample(int Index) const;
	void	SetSample(int Index, int Sample);
	array_view<const unsigned char> GetSamples() const;		// // //
	void	SetSamples(array_view<const unsigned char> Wave);		// // //

	int		GetModulation(int Index) const;
	void	SetModulation(int Index, int Value);
	array_view<const unsigned char> GetModTable() const;		// // //
	void	SetModTable(array_view<const unsigned char> Mod);		// // //

	int		GetModulationSpeed() const;
	void	SetModulationSpeed(int Speed);
	int		GetModulationDepth() const;
	void	SetModulationDepth(int Depth);
	int		GetModulationDelay() const;
	void	SetModulationDelay(int Delay);
	bool	GetModulationEnable() const;
	void	SetModulationEnable(bool Enable);

protected:
	void	CloneFrom(const CInstrument *pInst) override;		// // //

private:
	void	OnBlankInstrument() override;		// // //

public:
	static const int WAVE_SIZE = 64;
	static const int MOD_SIZE = 32;
	static const int SEQUENCE_COUNT = 3;		// // //
	static constexpr std::string_view SEQUENCE_NAME[] = {"Volume", "Arpeggio", "Pitch", "Hi-pitch", "Wave"};
	std::string_view GetSequenceName(int Index) const override { return SEQUENCE_NAME[Index]; }		// // //

private:
	// Instrument data
	std::unordered_map<sequence_t, std::shared_ptr<CSequence>> m_pSequence;
	std::array<unsigned char, WAVE_SIZE> m_iSamples = { };		// // //
	std::array<unsigned char, MOD_SIZE> m_iModulation = { };
	int			  m_iModulationSpeed;
	int			  m_iModulationDepth;
	int			  m_iModulationDelay;
	bool		  m_bModulationEnable;

public: // // // porting CSeqInstrument
	bool	GetSeqEnable(sequence_t SeqType) const override;
	void	SetSeqEnable(sequence_t SeqType, bool Enable) override;
	int		GetSeqIndex(sequence_t SeqType) const override;
	void	SetSeqIndex(sequence_t SeqType, int Value) override;
	std::shared_ptr<CSequence> GetSequence(sequence_t SeqType) const override;		// // //
	void	SetSequence(sequence_t SeqType, std::shared_ptr<CSequence> pSeq) override;		// // //
};
