#pragma once

#include <wx/wxprec.h>
#include <wx/aui/aui.h>

class BlockCipherFrame : public wxFrame
{
private:

protected:
	wxMenuBar* m_menubar1;
	wxMenu* m_menu1;
	wxMenu* m_menu3;
	wxMenu* m_menu31;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textCtrl1;
	wxRadioButton* m_radioBtn1;
	wxRadioButton* m_radioBtn2;
	wxRadioButton* m_radioBtn3;
	wxStaticText* m_staticText2;
	wxChoice* m_choice1;
	wxStaticText* m_staticText3;
	wxChoice* m_choice2;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_textCtrl2;
	wxTextCtrl* m_textCtrl3;
	wxCheckBox* m_checkBox1;
	wxButton* m_button1;
	wxButton* m_button2;
	wxStatusBar* m_statusBar1;

public:

	BlockCipherFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Block Cipher Encryption"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(520, 410), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP | wxSYSTEM_MENU | wxTAB_TRAVERSAL);

	~BlockCipherFrame();
};

