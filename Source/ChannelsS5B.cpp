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

// Sunsoft 5B (YM2149/AY-3-8910)

#include "ChannelsS5B.h"
#include "ChipHandlerS5B.h"		// // //
#include "APU/Types.h"		// // //
#include "APU/APUInterface.h"		// // //
#include "Instrument.h"		// // //
#include "InstHandler.h"		// // //
#include "SeqInstHandlerS5B.h"		// // //
#include "SongState.h"		// // //

// Class functions

void CChannelHandlerS5B::UpdateAutoEnvelope(int Period)		// // // 050B
{
	if (m_bEnvelopeEnabled && m_iAutoEnvelopeShift) {
		if (m_iAutoEnvelopeShift > 8) {
			Period >>= m_iAutoEnvelopeShift - 8 - 1;		// // // round off
			if (Period & 0x01) ++Period;
			Period >>= 1;
		}
		else if (m_iAutoEnvelopeShift < 8)
			Period <<= 8 - m_iAutoEnvelopeShift;
		chip_handler_.SetEnvelopePeriod(Period);
	}
}

// Instance functions

CChannelHandlerS5B::CChannelHandlerS5B(stChannelID ch, CChipHandlerS5B &parent) :		// // //
	CChannelHandler(ch, 0xFFF, 0x0F),
	chip_handler_(parent),		// // //
	m_bEnvelopeEnabled(false),		// // // 050B
	m_iAutoEnvelopeShift(0),		// // // 050B
	m_bUpdate(false)
{
	m_iDefaultDuty = value_cast(s5b_mode_t::Square);		// // //
}

bool CChannelHandlerS5B::HandleEffect(ft0cc::doc::effect_command cmd)
{
	switch (cmd.fx) {
	case ft0cc::doc::effect_type::SUNSOFT_NOISE: // W
		chip_handler_.SetDefaultNoisePeriod(cmd.param & 0x1Fu);		// // // 050B
		break;
	case ft0cc::doc::effect_type::SUNSOFT_ENV_HI: // I
		chip_handler_.SetEnvelopePeriod((cmd.param << 8) | (chip_handler_.GetEnvelopePeriod() & 0x00FFu));
		break;
	case ft0cc::doc::effect_type::SUNSOFT_ENV_LO: // J
		chip_handler_.SetEnvelopePeriod(cmd.param | (chip_handler_.GetEnvelopePeriod() & 0xFF00u));
		break;
	case ft0cc::doc::effect_type::SUNSOFT_ENV_TYPE: // H
		chip_handler_.SetEnvelopeShape(cmd.param & 0x0Fu);
		m_bUpdate = true;
		m_bEnvelopeEnabled = cmd.param != 0;
		m_iAutoEnvelopeShift = cmd.param >> 4;
		break;
	case ft0cc::doc::effect_type::DUTY_CYCLE:
		m_iDefaultDuty = m_iDutyPeriod = (cmd.param << 6) | ((cmd.param & 0x04) << 3);		// // // 050B
//		m_iDefaultDuty = m_iDutyPeriod = cmd.param;		// // //
		break;
	default: return CChannelHandler::HandleEffect(cmd);
	}

	return true;
}

void CChannelHandlerS5B::HandleNote(int MidiNote)		// // //
{
	CChannelHandler::HandleNote(MidiNote);
	chip_handler_.RestoreNoisePeriod();
}

void CChannelHandlerS5B::HandleEmptyNote()
{
}

void CChannelHandlerS5B::HandleCut()
{
	CutNote();
	m_iDutyPeriod = value_cast(s5b_mode_t::Square);
	m_iNote = -1;
}

void CChannelHandlerS5B::HandleRelease()
{
	if (!m_bRelease)
		ReleaseNote();		// // //
}

bool CChannelHandlerS5B::CreateInstHandler(inst_type_t Type)
{
	switch (Type) {
	case INST_2A03: case INST_VRC6: case INST_N163: case INST_S5B: case INST_FDS:
		switch (m_iInstTypeCurrent) {
		case INST_2A03: case INST_VRC6: case INST_N163: case INST_S5B: case INST_FDS: break;
		default:
			m_pInstHandler = std::make_unique<CSeqInstHandlerS5B>(this, 0x0F, Type == INST_S5B ? 0x40 : 0);
			return true;
		}
	}
	return false;
}

void CChannelHandlerS5B::WriteReg(int Reg, int Value)
{
	m_pAPU->Write(0xC000, Reg);
	m_pAPU->Write(0xE000, Value);
}

void CChannelHandlerS5B::ResetChannel()
{
	CChannelHandler::ResetChannel();

	m_iDefaultDuty = m_iDutyPeriod = value_cast(s5b_mode_t::Square);
	m_bEnvelopeEnabled = false;
	m_iAutoEnvelopeShift = 0;
}

int CChannelHandlerS5B::CalculateVolume() const		// // //
{
	return LimitVolume((m_iVolume >> VOL_COLUMN_SHIFT) + m_iInstVolume - 15 - GetTremolo());
}

int CChannelHandlerS5B::ConvertDuty(int Duty) const		// // //
{
	switch (m_iInstTypeCurrent) {
	case INST_2A03: case INST_VRC6: case INST_N163:
		return value_cast(s5b_mode_t::Square);
	default:
		return Duty;
	}
}

void CChannelHandlerS5B::ClearRegisters()
{
	WriteReg(8 + GetChannelID().Subindex, 0);		// Clear volume
}

std::string CChannelHandlerS5B::GetCustomEffectString() const		// // //
{
	return chip_handler_.GetCustomEffectString();
}

void CChannelHandlerS5B::RefreshChannel()
{
	int Period = CalculatePeriod();
	unsigned char LoPeriod = Period & 0xFF;
	unsigned char HiPeriod = Period >> 8;
	int Volume = CalculateVolume();

	unsigned char Noise = (m_bGate && (m_iDutyPeriod & value_cast(s5b_mode_t::Noise))) ? 0 : 1;
	unsigned char Square = (m_bGate && (m_iDutyPeriod & value_cast(s5b_mode_t::Square))) ? 0 : 1;
	unsigned char Envelope = (m_bGate && (m_iDutyPeriod & value_cast(s5b_mode_t::Envelope))) ? 0x10 : 0; // m_bEnvelopeEnabled ? 0x10 : 0;

	UpdateAutoEnvelope(Period);		// // // 050B
	chip_handler_.SetChannelOutput(GetChannelID().Subindex, Square, Noise);

	unsigned subindex = GetChannelID().Subindex;		// // //
	WriteReg(subindex * 2    , LoPeriod);
	WriteReg(subindex * 2 + 1, HiPeriod);
	WriteReg(subindex + 8    , Volume | Envelope);

	if (Envelope && (m_bTrigger || m_bUpdate))		// // // 050B
		chip_handler_.TriggerEnvelope();
	m_bUpdate = false;
}

void CChannelHandlerS5B::SetNoiseFreq(int Pitch)		// // //
{
	chip_handler_.SetNoisePeriod(Pitch);
}
