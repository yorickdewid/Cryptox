///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EXUIFRAME_H__
#define __EXUIFRAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class BaseBlockCipherFrame
///////////////////////////////////////////////////////////////////////////////
class BaseBlockCipherFrame : public wxFrame 
{
	private:
	
	protected:
		enum
		{
			wxID_Encrypt = 1000,
			wxID_Decrypt
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
	
	public:
		
		BaseBlockCipherFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Block Cipher Encryption"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 520,600 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		
		~BaseBlockCipherFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class BaseHashFrame
///////////////////////////////////////////////////////////////////////////////
class BaseHashFrame : public wxFrame 
{
	private:
	
	protected:
		enum
		{
			wxID_Hash = 1000
		};
		
		wxMenuBar* m_menubar;
		wxMenu* m_menu1;
		wxMenu* m_menu31;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_txtInput;
		wxRadioButton* m_radioBtn1;
		wxRadioButton* m_radioBtn2;
		wxRadioButton* m_radioBtn3;
		wxStaticText* m_staticText2;
		wxChoice* m_choice1;
		wxTextCtrl* m_txtOutput;
		wxCheckBox* m_chkHex;
		wxButton* m_btnHash;
		wxStatusBar* m_statusBar;
	
	public:
		
		BaseHashFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Hash Calculation"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 532,290 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		
		~BaseHashFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class BaseDataViewer
///////////////////////////////////////////////////////////////////////////////
class BaseDataViewer : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText19;
		wxChoice* m_choice7;
		wxButton* m_btnClipboard;
		wxButton* m_btnSave;
		wxTextCtrl* m_txtData;
	
	public:
		
		BaseDataViewer( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Data Viewer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,360 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER ); 
		~BaseDataViewer();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class BaseCertificateManager
///////////////////////////////////////////////////////////////////////////////
class BaseCertificateManager : public wxFrame 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxDataViewCtrl* m_dataViewCtrl1;
		wxDataViewListCtrl* m_dataViewListCtrl1;
		wxPanel* m_panel2;
		wxDataViewCtrl* m_dataViewCtrl11;
		wxDataViewListCtrl* m_dataViewListCtrl11;
		wxPanel* m_panel3;
		wxDataViewCtrl* m_dataViewCtrl111;
		wxDataViewListCtrl* m_dataViewListCtrl111;
		wxMenuBar* m_menubar3;
		wxMenu* m_menu8;
		wxMenu* m_menu9;
	
	public:
		
		BaseCertificateManager( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Certificate Manager"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,400 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~BaseCertificateManager();
	
};

#endif //__EXUIFRAME_H__
