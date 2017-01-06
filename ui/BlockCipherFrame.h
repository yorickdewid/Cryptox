#pragma once

#include <wx/wxprec.h>
#include <wx/aui/aui.h>

class BlockCipherFrame : public wxFrame
{
private:

	enum
	{
		wxID_Encrypt,
		wxID_Decrypt,
	};

	wxMenuBar* m_menubar;
	wxMenu* m_menu1;
	wxMenu* m_menu3;
	wxMenu* m_menu5;
	wxMenu* m_menu31;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_txtInput;
	wxRadioButton* m_radioBtn1;
	wxRadioButton* m_radioBtn2;
	wxRadioButton* m_radioBtn3;
	wxStaticText* m_staticText2;
	wxChoice* m_choice1;
	wxStaticText* m_staticText3;
	wxChoice* m_choice2;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_txtKey;
	wxStaticText* m_staticText5;
	wxTextCtrl* m_txtIV;
	wxStaticText* m_staticText6;
	wxTextCtrl* m_txtAAD;
	wxTextCtrl* m_txtOutput;
	wxCheckBox* m_chkHex;
	wxButton* m_btnEncrypt;
	wxButton* m_btnDecrypt;
	wxStatusBar* m_statusBar;

	void OnEncrypt(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

public:
	BlockCipherFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Block Cipher Encryption"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(520, 610), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP | wxSYSTEM_MENU | wxTAB_TRAVERSAL);
	~BlockCipherFrame();
};

