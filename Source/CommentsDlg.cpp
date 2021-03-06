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

#include "CommentsDlg.h"
#include "str_conv/str_conv.hpp"		// // //

// CCommentsDlg dialog

IMPLEMENT_DYNAMIC(CCommentsDlg, CDialog)

CCommentsDlg::CCommentsDlg(CWnd* pParent /*=NULL*/) :
	CDialog(CCommentsDlg::IDD, pParent),
	m_sComment(""), m_bShowOnLoad(false)		// // //
{
}

const std::string &CCommentsDlg::GetComment() const		// // //
{
	return m_sComment;
}

void CCommentsDlg::SetComment(std::string_view Str)		// // //
{
	m_sComment = Str;
}

bool CCommentsDlg::GetShowOnLoad() const
{
	return m_bShowOnLoad;
}

void CCommentsDlg::SetShowOnLoad(bool Enable)
{
	m_bShowOnLoad = Enable;
}

bool CCommentsDlg::IsChanged() const
{
	return m_bChanged;
}

void CCommentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCommentsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_COMMENTS, OnEnChangeComments)
	ON_BN_CLICKED(IDC_SHOWONOPEN, OnBnClickedShowonopen)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CCommentsDlg message handlers

void CCommentsDlg::SaveComment()
{
	CStringW str;		// // //
	GetDlgItemTextW(IDC_COMMENTS, str);		// // //
	m_sComment = conv::to_utf8(str);
	m_bShowOnLoad = IsDlgButtonChecked(IDC_SHOWONOPEN) == BST_CHECKED;
}

void CCommentsDlg::OnBnClickedOk()
{
	SaveComment();
	CDialog::OnOK();		// // //
}

void CCommentsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pEdit = GetDlgItem(IDC_COMMENTS);
	CWnd *pOk = GetDlgItem(IDOK);
	CWnd *pCancel = GetDlgItem(IDCANCEL);
	CWnd *pCheckBox = GetDlgItem(IDC_SHOWONOPEN);

	CRect dlgRect;
	GetClientRect(dlgRect);

	if (pEdit != NULL) {
		dlgRect.bottom -= 39;
		pEdit->MoveWindow(dlgRect);
		CRect buttonRect;
		pOk->GetClientRect(buttonRect);
		buttonRect.MoveToY(dlgRect.bottom + 8);
		buttonRect.MoveToX(dlgRect.right - buttonRect.Width() * 2 - 10);
		pOk->MoveWindow(buttonRect);
		pCancel->GetClientRect(buttonRect);
		buttonRect.MoveToY(dlgRect.bottom + 8);
		buttonRect.MoveToX(dlgRect.right - buttonRect.Width() - 7);
		pCancel->MoveWindow(buttonRect);
		pCheckBox->GetClientRect(buttonRect);
		buttonRect.MoveToY(dlgRect.bottom + 14);
		buttonRect.MoveToX(8);
		pCheckBox->MoveWindow(buttonRect);
	}
}

BOOL CCommentsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemTextW(IDC_COMMENTS, conv::to_wide(m_sComment).data());		// // //
	CheckDlgButton(IDC_SHOWONOPEN, m_bShowOnLoad ? BST_CHECKED : BST_UNCHECKED);
	m_bChanged = false;

	m_cFont.CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Courier");		// // //
	GetDlgItem(IDC_COMMENTS)->SetFont(&m_cFont);

	if (WinRect.top == 0 && WinRect.bottom == 0)
		GetWindowRect(&WinRect);
	else
		MoveWindow(WinRect.left, WinRect.top, WinRect.right - WinRect.left, WinRect.bottom - WinRect.top);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCommentsDlg::DestroyWindow()
{
	GetWindowRect(&WinRect);
	return CDialog::DestroyWindow();
}

void CCommentsDlg::OnEnChangeComments()
{
	m_bChanged = true;
}

void CCommentsDlg::OnBnClickedShowonopen()
{
	m_bChanged = true;
}

void CCommentsDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 200;

	CDialog::OnGetMinMaxInfo(lpMMI);
}
