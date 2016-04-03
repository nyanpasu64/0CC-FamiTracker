/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2014  Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2015 HertzDevil
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

//
// Derived channels, 2A03
//

class CChannelHandler2A03 : public CChannelHandler {
public:
	CChannelHandler2A03();
	virtual void ResetChannel();

protected:
	virtual void HandleNoteData(stChanNote *pNoteData, int EffColumns);
	virtual void HandleCustomEffects(effect_t EffNum, int EffParam);
	virtual void HandleEmptyNote();
	virtual void HandleNote(int Note, int Octave);
	void         HandleCut();
	void         HandleRelease();
	bool         CreateInstHandler(inst_type_t Type);		// // //

protected:
	// // //
	bool	m_bHardwareEnvelope;	// // // (constant volume flag, bit 4)
	bool	m_bEnvelopeLoop;		// // // (halt length counter flag, bit 5 / triangle bit 7)
	bool	m_bResetEnvelope;		// // //
	int		m_iLengthCounter;		// // //
};

// // // 2A03 Square
class C2A03Square : public CChannelHandler2A03 {
public:
	C2A03Square();
	void RefreshChannel();
	void SetChannelID(int ID);		// // //
protected:
	int ConvertDuty(int Duty) const;		// // //
	void ClearRegisters();

	void HandleNoteData(stChanNote *pNoteData, int EffColumns);
	void HandleCustomEffects(effect_t EffNum, int EffParam);
	void HandleEmptyNote();
	void HandleNote(int Note, int Octave);
	CString GetCustomEffectString() const;		// // //

	unsigned char m_iChannel;		// // //
	unsigned char m_cSweep;
	bool	m_bSweeping;
	int		m_iSweep;
	int		m_iLastPeriod;
};

// Triangle
class CTriangleChan : public CChannelHandler2A03 {
public:
	CTriangleChan();
	void RefreshChannel();
	void ResetChannel();		// // //
protected:
	void HandleCustomEffects(effect_t EffNum, int EffParam);		// // //
	void ClearRegisters();
	CString GetCustomEffectString() const;		// // //
private:
	int m_iLinearCounter;
};

// Noise
class CNoiseChan : public CChannelHandler2A03 {
public:
	CNoiseChan();
	void RefreshChannel();
protected:
	void ClearRegisters();
	CString GetCustomEffectString() const;		// // //
	void HandleNote(int Note, int Octave);
	void SetupSlide();		// // //

	int LimitPeriod(int Period) const;		// // //

	int TriggerNote(int Note);
};

// DPCM
class CDPCMChan : public CChannelHandler {		// // //
public:
	CDPCMChan(CSampleMem *pSampleMem);
	void RefreshChannel();
protected:
	void HandleNoteData(stChanNote *pNoteData, int EffColumns);
	void HandleCustomEffects(effect_t EffNum, int EffParam);
	bool HandleInstrument(int Instrument, bool Trigger, bool NewInstrument);
	void HandleEmptyNote();
	void HandleCut();
	void HandleRelease();
	void HandleNote(int Note, int Octave);

	void ClearRegisters();
	CString GetCustomEffectString() const;		// // //
private:
	// DPCM variables
	CSampleMem *m_pSampleMem;

	unsigned char m_cDAC;
	unsigned char m_iLoop;
	unsigned char m_iOffset;
	unsigned char m_iSampleLength;
	unsigned char m_iLoopOffset;
	unsigned char m_iLoopLength;
	int m_iRetrigger;
	int m_iRetriggerCntr;
	int m_iCustomPitch;
	bool m_bTrigger;
	bool m_bEnabled;
};
