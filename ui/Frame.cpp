#include "Frame.h"
#include "SettingsPanel.h"
#include "SecretListModel.h"
#include "RandonGenerator.h"

#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/spinctrl.h>
#include <wx/aboutdlg.h>
#include <wx/dataview.h>

Frame::Frame(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	// AUI manage this frame
	m_mgr.SetManagedWindow(this);

	// Set frame min size and icon
	SetIcon(wxIcon(wxString("cryptox.ico"), wxBITMAP_TYPE_ICO));
	SetMinSize(wxSize(800, 700));

	// Set border size
	GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);

	// Set up default notebook style
	m_notebook_style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
	m_notebook_theme = 0;

	// Create menu
	CreateMenu();
	SetMenuBar(CreateMenuBar());

	// Create statusbar
	CreateStatusBar();
	SetStatusText(wxT("Ready"));

	// Create toolbar
	wxAuiToolBarItemArray prepend_items;
	wxAuiToolBarItemArray append_items;
	wxAuiToolBarItem item;
	item.SetKind(wxITEM_SEPARATOR);
	append_items.Add(item);
	item.SetKind(wxITEM_NORMAL);
	item.SetId(ID_CustomizeToolbar);
	item.SetLabel(wxT("Customize..."));
	append_items.Add(item);

	wxBitmap tb2_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));
	wxBitmap tb3_bmp1 = wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
	wxBitmap tb4_bmp1 = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));

	wxAuiToolBar *maintb = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | 
		wxAUI_TB_OVERFLOW |
		wxAUI_TB_HORIZONTAL);
	maintb->SetToolBitmapSize(wxSize(16, 16));
	maintb->AddTool(ID_SampleItem + 6, wxT("Disabled"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 7, wxT("Test"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 8, wxT("Test"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 9, wxT("Test"), tb2_bmp1);
	maintb->AddSeparator();
	maintb->AddTool(ID_SampleItem + 10, wxT("Test"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 11, wxT("Test"), tb2_bmp1);
	maintb->AddSeparator();
	maintb->AddTool(ID_SampleItem + 12, wxT("Test"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 13, wxT("Test"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 14, wxT("Test"), tb2_bmp1);
	maintb->AddTool(ID_SampleItem + 15, wxT("Test"), tb2_bmp1);
	maintb->AddSeparator();
	maintb->AddTool(ID_SampleItem + 16, wxT("Check 1"), tb3_bmp1, wxT("Check 1"), wxITEM_CHECK);
	maintb->AddTool(ID_SampleItem + 17, wxT("Check 2"), tb3_bmp1, wxT("Check 2"), wxITEM_CHECK);
	maintb->AddTool(ID_SampleItem + 18, wxT("Check 3"), tb3_bmp1, wxT("Check 3"), wxITEM_CHECK);
	maintb->AddTool(ID_SampleItem + 19, wxT("Check 4"), tb3_bmp1, wxT("Check 4"), wxITEM_CHECK);
	maintb->AddSeparator();
	maintb->AddTool(ID_SampleItem + 20, wxT("Radio 1"), tb3_bmp1, wxT("Radio 1"), wxITEM_RADIO);
	maintb->AddTool(ID_SampleItem + 21, wxT("Radio 2"), tb3_bmp1, wxT("Radio 2"), wxITEM_RADIO);
	maintb->AddTool(ID_SampleItem + 22, wxT("Radio 3"), tb3_bmp1, wxT("Radio 3"), wxITEM_RADIO);
	maintb->AddSeparator();
	maintb->AddTool(ID_SampleItem + 23, wxT("Radio 1 (Group 2)"), tb3_bmp1, wxT("Radio 1 (Group 2)"), wxITEM_RADIO);
	maintb->AddTool(ID_SampleItem + 24, wxT("Radio 2 (Group 2)"), tb3_bmp1, wxT("Radio 2 (Group 2)"), wxITEM_RADIO);
	maintb->AddTool(ID_SampleItem + 25, wxT("Radio 3 (Group 2)"), tb3_bmp1, wxT("Radio 3 (Group 2)"), wxITEM_RADIO);
	//tb2->SetCustomOverflowItems(prepend_items, append_items);
	maintb->EnableTool(ID_SampleItem + 6, false);
	maintb->Realize();

	wxAuiToolBar *subtb = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE |
		wxAUI_TB_OVERFLOW |
		wxAUI_TB_TEXT |
		wxAUI_TB_HORZ_TEXT);
	subtb->SetToolBitmapSize(wxSize(16, 16));
	subtb->AddTool(ID_DropDownToolbarItem, wxT("Item 1"), tb4_bmp1);
	subtb->AddTool(ID_SampleItem + 23, wxT("Item 2"), tb4_bmp1);
	subtb->AddTool(ID_SampleItem + 24, wxT("Item 3"), tb4_bmp1);
	subtb->AddTool(ID_SampleItem + 25, wxT("Item 4"), tb4_bmp1);
	subtb->AddSeparator();
	subtb->AddTool(ID_SampleItem + 26, wxT("Item 5"), tb4_bmp1);
	subtb->AddTool(ID_SampleItem + 27, wxT("Item 6"), tb4_bmp1);
	subtb->AddTool(ID_SampleItem + 28, wxT("Item 7"), tb4_bmp1);
	subtb->AddTool(ID_SampleItem + 29, wxT("Item 8"), tb4_bmp1);
	subtb->SetToolDropDown(ID_DropDownToolbarItem, true);
	subtb->SetCustomOverflowItems(prepend_items, append_items);
	wxChoice* choice = new wxChoice(subtb, ID_SampleItem + 35);
	choice->AppendString(wxT("One choice"));
	choice->AppendString(wxT("Another choice"));
	subtb->AddControl(choice);
	subtb->Realize();

	// add a bunch of panes
	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test1")).Caption(wxT("Pane Caption")).
		Top());

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test2")).Caption(wxT("Client Size Reporter")).
		Bottom().Position(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test3")).Caption(wxT("Client Size Reporter")).
		Bottom().
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test4")).Caption(wxT("Pane Caption")).
		Left());

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test5")).Caption(wxT("No Close Button")).
		Right().CloseButton(false));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test6")).Caption(wxT("Client Size Reporter")).
		Right().Row(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test7")).Caption(wxT("Client Size Reporter")).
		Left().Layer(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
		Name(wxT("test8")).Caption(wxT("Primitives")).
		Left().Layer(1).Position(1).
		CloseButton(false));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test9")).Caption(wxT("Min Size 200x100")).
		BestSize(wxSize(200, 100)).MinSize(wxSize(200, 100)).
		Bottom().Layer(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreatePropCtrl(), wxAuiPaneInfo().
		Name(wxT("test17")).
		Layer(1).Right().
		CloseButton(false).CaptionVisible(false));

	m_output = CreateTextCtrl(wxT("Primitives loaded"));
	m_mgr.AddPane(m_output, wxAuiPaneInfo().
		Name(wxT("test10")).Caption(wxT("Output")).
		Bottom().Layer(1).Position(1).MinimizeButton());

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test11")).Caption(wxT("Fixed Pane")).
		Bottom().Layer(1).Position(2).Fixed());


	m_mgr.AddPane(new SettingsPanel(this, this), wxAuiPaneInfo().
		Name(wxT("settings")).Caption(wxT("Dock Manager Settings")).
		Dockable(false).Float().Hide());

	// Set notebook on center page
	m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name(wxT("notebook_content")).
		CenterPane().PaneBorder(false));

	// Add toolbars to frame
	m_mgr.AddPane(maintb, wxAuiPaneInfo().
		Name(wxT("maintb")).
		ToolbarPane().Top().Row(1).Floatable(false).
		Gripper(false));

	m_mgr.AddPane(subtb, wxAuiPaneInfo().
		Name(wxT("subtb")).
		ToolbarPane().Top().Row(2).Floatable(false).
		Gripper(false));

	// make some default perspectives

	wxString perspective_all = m_mgr.SavePerspective();

	int i, count;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	for (i = 0, count = all_panes.GetCount(); i < count; ++i)
		if (!all_panes.Item(i).IsToolbar())
			all_panes.Item(i).Hide();
	m_mgr.GetPane(wxT("test8")).Show().Left().Layer(0).Row(0).Position(0);
	m_mgr.GetPane(wxT("test10")).Show().Bottom().Layer(0).Row(0).Position(0);
	m_mgr.GetPane(wxT("test17")).Show().Layer(1).Right().Position(0);
	m_mgr.GetPane(wxT("notebook_content")).Show();
	wxString perspective_default = m_mgr.SavePerspective();

	m_perspectives.Add(perspective_default);
	m_perspectives.Add(perspective_all);

	// Commit changes to wxAuiManager
	m_mgr.Update();
}

Frame::~Frame()
{
	m_mgr.UnInit();
}

wxAuiDockArt *Frame::GetDockArt()
{
	return m_mgr.GetArtProvider();
}

void Frame::DoUpdate()
{
	m_mgr.Update();
}

void Frame::OnSettings(wxCommandEvent& WXUNUSED(evt))
{
	// show the settings pane, and float it
	wxAuiPaneInfo& floating_pane = m_mgr.GetPane(wxT("settings")).Float().Show();

	if (floating_pane.floating_pos == wxDefaultPosition)
		floating_pane.FloatingPosition(GetStartPosition());

	m_mgr.Update();
}
#include "BlockCipherFrame.h"
void Frame::CreatePrimitiveFrame()
{
	wxFrame *frame = new BlockCipherFrame(this);
	frame->SetIcon(wxIcon(wxString("unlocked.ico"), wxBITMAP_TYPE_ICO));
	frame->Show();
	frame->SetFocus();
}

void Frame::OnCustomizeToolbar(wxCommandEvent& WXUNUSED(evt))
{
	wxMessageBox(wxT("Customize Toolbar clicked"));
}

void Frame::OnGradient(wxCommandEvent& event)
{
	int gradient = 0;

	switch (event.GetId())
	{
		case ID_NoGradient:         gradient = wxAUI_GRADIENT_NONE; break;
		case ID_VerticalGradient:   gradient = wxAUI_GRADIENT_VERTICAL; break;
		case ID_HorizontalGradient: gradient = wxAUI_GRADIENT_HORIZONTAL; break;
	}

	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, gradient);
	m_mgr.Update();
}

void Frame::OnToolbarResizing(wxCommandEvent& WXUNUSED(evt))
{
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	const size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; ++i)
	{
		wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
		if (toolbar)
		{
			all_panes[i].Resizable(!all_panes[i].IsResizable());
		}
	}

	m_mgr.Update();
}

void Frame::OnManagerFlag(wxCommandEvent& event)
{
	unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
	if (event.GetId() == ID_TransparentDrag ||
		event.GetId() == ID_TransparentHint ||
		event.GetId() == ID_HintFade)
	{
		wxMessageBox(wxT("This option is presently only available on wxGTK, wxMSW and wxMac"));
		return;
	}
#endif

	int id = event.GetId();

	if (id == ID_TransparentHint ||
		id == ID_VenetianBlindsHint ||
		id == ID_RectangleHint ||
		id == ID_NoHint)
	{
		unsigned int flags = m_mgr.GetFlags();
		flags &= ~wxAUI_MGR_TRANSPARENT_HINT;
		flags &= ~wxAUI_MGR_VENETIAN_BLINDS_HINT;
		flags &= ~wxAUI_MGR_RECTANGLE_HINT;
		m_mgr.SetFlags(flags);
	}

	switch (id)
	{
		case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
		case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
		case ID_HintFade: flag = wxAUI_MGR_HINT_FADE; break;
		case ID_NoVenetianFade: flag = wxAUI_MGR_NO_VENETIAN_BLINDS_FADE; break;
		case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
		case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
		case ID_VenetianBlindsHint: flag = wxAUI_MGR_VENETIAN_BLINDS_HINT; break;
		case ID_RectangleHint: flag = wxAUI_MGR_RECTANGLE_HINT; break;
		case ID_LiveUpdate: flag = wxAUI_MGR_LIVE_RESIZE; break;
	}

	if (flag)
	{
		m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);
	}

	m_mgr.Update();
}

void Frame::OnUpdateUI(wxUpdateUIEvent& event)
{
	unsigned int flags = m_mgr.GetFlags();

	switch (event.GetId())
	{
		case ID_NoGradient:
			event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE);
			break;
		case ID_VerticalGradient:
			event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL);
			break;
		case ID_HorizontalGradient:
			event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL);
			break;
		case ID_AllowToolbarResizing:
		{
			wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
			const size_t count = all_panes.GetCount();
			for (size_t i = 0; i < count; ++i)
			{
				wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
				if (toolbar)
				{
					event.Check(all_panes[i].IsResizable());
					break;
				}
			}
			break;
		}
		case ID_AllowFloating:
			event.Check((flags & wxAUI_MGR_ALLOW_FLOATING) != 0);
			break;
		case ID_TransparentDrag:
			event.Check((flags & wxAUI_MGR_TRANSPARENT_DRAG) != 0);
			break;
		case ID_TransparentHint:
			event.Check((flags & wxAUI_MGR_TRANSPARENT_HINT) != 0);
			break;
		case ID_LiveUpdate:
			event.Check((flags & wxAUI_MGR_LIVE_RESIZE) != 0);
			break;
		case ID_VenetianBlindsHint:
			event.Check((flags & wxAUI_MGR_VENETIAN_BLINDS_HINT) != 0);
			break;
		case ID_RectangleHint:
			event.Check((flags & wxAUI_MGR_RECTANGLE_HINT) != 0);
			break;
		case ID_NoHint:
			event.Check(((wxAUI_MGR_TRANSPARENT_HINT |
				wxAUI_MGR_VENETIAN_BLINDS_HINT |
				wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
			break;
		case ID_HintFade:
			event.Check((flags & wxAUI_MGR_HINT_FADE) != 0);
			break;
		case ID_NoVenetianFade:
			event.Check((flags & wxAUI_MGR_NO_VENETIAN_BLINDS_FADE) != 0);
			break;
	}
}

void Frame::OnPaneClose(wxAuiManagerEvent& evt)
{
	if (evt.pane->name == wxT("test10"))
	{
		int res = wxMessageBox(wxT("Are you sure you want to close/hide this pane?"),
			wxT("wxAUI"),
			wxYES_NO,
			this);
		if (res != wxYES)
			evt.Veto();
	}
}

void Frame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
	wxTextEntryDialog dlg(this, wxT("Enter a name for the new perspective:"),
		wxT("wxAUI Test"));

	dlg.SetValue(wxString::Format(wxT("Perspective %u"), unsigned(m_perspectives.GetCount() + 1)));
	if (dlg.ShowModal() != wxID_OK)
		return;

	if (m_perspectives.GetCount() == 0)
	{
		m_perspectives_menu->AppendSeparator();
	}

	m_perspectives_menu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
	m_perspectives.Add(m_mgr.SavePerspective());
}

void Frame::OnCopyPerspectiveCode(wxCommandEvent& WXUNUSED(evt))
{
	wxString s = m_mgr.SavePerspective();

#if wxUSE_CLIPBOARD
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(s));
		wxTheClipboard->Close();
	}
#endif
}

void Frame::OnRestorePerspective(wxCommandEvent& evt)
{
	m_mgr.LoadPerspective(m_perspectives.Item(evt.GetId() - ID_FirstPerspective));
}

void Frame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
	wxAuiNotebook *ctrl = (wxAuiNotebook*)evt.GetEventObject();
	if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
	{
		int res = wxMessageBox(wxT("Are you sure you want to close/hide this notebook page?"),
			wxT("wxAUI"),
			wxYES_NO,
			this);
		if (res != wxYES)
			evt.Veto();
	}
}

void Frame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
	wxAuiNotebook *ctrl = (wxAuiNotebook *)evt.GetEventObject();

	// selection should always be a valid index
	wxASSERT_MSG(ctrl->GetSelection() < (int)ctrl->GetPageCount(),
		wxString::Format("Invalid selection %d, only %d pages left",
			ctrl->GetSelection(),
			(int)ctrl->GetPageCount()));

	evt.Skip();
}

void Frame::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
	// for the purpose of this test application, explicitly
	// allow all notebook drag and drop events
	evt.Allow();
}

wxPoint Frame::GetStartPosition()
{
	static int x = 0;
	x += 20;
	wxPoint pt = ClientToScreen(wxPoint(0, 0));
	return wxPoint(pt.x + x, pt.y + x);
}

void Frame::OnCreateTree(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
		Caption(wxT("Tree Control")).
		Float().FloatingPosition(GetStartPosition()).
		FloatingSize(wxSize(150, 300)));
	m_mgr.Update();
}

void Frame::OnCreateGrid(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().
		Caption(wxT("Grid")).
		Float().FloatingPosition(GetStartPosition()).
		FloatingSize(wxSize(300, 200)));
	m_mgr.Update();
}

void Frame::OnCreateHTML(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().
		Caption(wxT("HTML Control")).
		Float().FloatingPosition(GetStartPosition()).
		FloatingSize(wxSize(300, 200)));
	m_mgr.Update();
}

void Frame::OnCreateNotebook(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().
		Caption(wxT("Notebook")).
		Float().FloatingPosition(GetStartPosition()).
		//FloatingSize(300,200).
		CloseButton(true).MaximizeButton(true));
	m_mgr.Update();
}

void Frame::OnCreateText(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().
		Caption(wxT("Text Control")).
		Float().FloatingPosition(GetStartPosition()));
	m_mgr.Update();
}

void Frame::OnCreateSizeReport(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Caption(wxT("Client Size Reporter")).
		Float().FloatingPosition(GetStartPosition()).
		CloseButton(true).MaximizeButton(true));
	m_mgr.Update();
}

void Frame::OnDropDownToolbarItem(wxAuiToolBarEvent& evt)
{
	if (evt.IsDropDownClicked())
	{
		wxAuiToolBar *tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

		tb->SetToolSticky(evt.GetId(), true);

		// create the popup menu
		wxMenu menuPopup;

		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));

		wxMenuItem* m1 = new wxMenuItem(&menuPopup, 10001, _("Drop Down Item 1"));
		m1->SetBitmap(bmp);
		menuPopup.Append(m1);

		wxMenuItem* m2 = new wxMenuItem(&menuPopup, 10002, _("Drop Down Item 2"));
		m2->SetBitmap(bmp);
		menuPopup.Append(m2);

		wxMenuItem* m3 = new wxMenuItem(&menuPopup, 10003, _("Drop Down Item 3"));
		m3->SetBitmap(bmp);
		menuPopup.Append(m3);

		wxMenuItem* m4 = new wxMenuItem(&menuPopup, 10004, _("Drop Down Item 4"));
		m4->SetBitmap(bmp);
		menuPopup.Append(m4);

		// line up our menu with the button
		wxRect rect = tb->GetToolRect(evt.GetId());
		wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
		pt = ScreenToClient(pt);


		PopupMenu(&menuPopup, pt);


		// make sure the button is "un-stuck"
		tb->SetToolSticky(evt.GetId(), false);
	}
}

void Frame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void Frame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(wxT("Cryptox"));
	info.SetDescription(wxT("Cryptography management and development studio"));
	info.SetCopyright(wxT("(C) 2017 Quenza Inc."));
	info.SetIcon(wxIcon(wxString("cryptox.ico"), wxBITMAP_TYPE_ICO));
	info.SetLicence("GPL 3");
	info.SetVersion("0.5", "Alpha version 0.5");
	info.SetWebSite("https://github.com/yorickdewid/Cryptox");
	info.AddDeveloper("Yorick de Wid");

	wxAboutBox(info);
}

wxMenuBar *Frame::CreateMenuBar()
{
	wxMenuBar *mb = new wxMenuBar;

	wxMenu *file_menu = new wxMenu;
	file_menu->Append(wxID_NEW);
	file_menu->Append(wxID_OPEN);
	file_menu->Append(wxID_CLOSE);
	file_menu->Append(wxID_SAVE);
	file_menu->Append(wxID_SAVEAS);
	file_menu->Append(wxID_REVERT, wxT("Re&vert..."));
	file_menu->AppendSeparator();
	file_menu->Append(wxID_EXIT, wxT("&Exit"));

	wxMenu *view_menu = new wxMenu;
	view_menu->Append(ID_CreateText, wxT("Create Text Control"));
	view_menu->Append(ID_CreateHTML, wxT("Create HTML Control"));
	view_menu->Append(ID_CreateTree, wxT("Create Tree"));
	view_menu->Append(ID_CreateGrid, wxT("Create Grid"));
	view_menu->Append(ID_CreateNotebook, wxT("Create Notebook"));
	view_menu->Append(ID_CreateSizeReport, wxT("Create Size Reporter"));

	wxMenu *options_menu = new wxMenu;
	options_menu->AppendRadioItem(ID_TransparentHint, wxT("Transparent Hint"));
	options_menu->AppendRadioItem(ID_VenetianBlindsHint, wxT("Venetian Blinds Hint"));
	options_menu->AppendRadioItem(ID_RectangleHint, wxT("Rectangle Hint"));
	options_menu->AppendRadioItem(ID_NoHint, wxT("No Hint"));
	options_menu->AppendSeparator();
	options_menu->AppendCheckItem(ID_HintFade, wxT("Hint Fade-in"));
	options_menu->AppendCheckItem(ID_AllowFloating, wxT("Allow Floating"));
	options_menu->AppendCheckItem(ID_NoVenetianFade, wxT("Disable Venetian Blinds Hint Fade-in"));
	options_menu->AppendCheckItem(ID_TransparentDrag, wxT("Transparent Drag"));
	options_menu->AppendCheckItem(ID_AllowActivePane, wxT("Allow Active Pane"));
	options_menu->AppendCheckItem(ID_LiveUpdate, wxT("Live Resize Update"));
	options_menu->AppendSeparator();
	options_menu->AppendRadioItem(ID_NoGradient, wxT("No Caption Gradient"));
	options_menu->AppendRadioItem(ID_VerticalGradient, wxT("Vertical Caption Gradient"));
	options_menu->AppendRadioItem(ID_HorizontalGradient, wxT("Horizontal Caption Gradient"));
	options_menu->AppendSeparator();
	options_menu->AppendCheckItem(ID_AllowToolbarResizing, wxT("Allow Toolbar Resizing"));
	
	wxMenu *tools_menu = new wxMenu;
	tools_menu->Append(wxID_ANY, wxT("Password Generator"));
	tools_menu->Append(ID_RandomGeneratorWindow, wxT("Block Cipher Encryption"));
	tools_menu->AppendSeparator();
	tools_menu->Append(ID_Settings, wxT("Settings Pane"));

	m_perspectives_menu = new wxMenu;
	m_perspectives_menu->Append(ID_CreatePerspective, wxT("Create Perspective"));
	m_perspectives_menu->Append(ID_CopyPerspectiveCode, wxT("Copy Perspective Data To Clipboard"));
	m_perspectives_menu->AppendSeparator();
	m_perspectives_menu->Append(ID_FirstPerspective + 0, wxT("Default Startup"));
	m_perspectives_menu->Append(ID_FirstPerspective + 1, wxT("All Panes"));

	wxMenu *help_menu = new wxMenu;
	help_menu->Append(wxID_ABOUT);

	mb->Append(file_menu, wxT("&File"));
	mb->Append(view_menu, wxT("&View"));
	mb->Append(m_perspectives_menu, wxT("&Perspectives"));
	mb->Append(options_menu, wxT("&Options"));
	mb->Append(tools_menu, wxT("&Tools"));
	mb->Append(help_menu, wxT("&Help"));

	return mb;
}

wxTextCtrl *Frame::CreateTextCtrl(const wxString& ctrl_text)
{
	wxString text;
	if (!ctrl_text.empty())
		text = ctrl_text;
	else
		text.Printf(wxT("This is text box %d"), 1);

	return new wxTextCtrl(this, wxID_ANY, text,
		wxPoint(0, 0), wxSize(150, 90),
		wxNO_BORDER | wxTE_MULTILINE);
}


wxGrid *Frame::CreateGrid()
{
	wxGrid* grid = new wxGrid(this, wxID_ANY,
		wxPoint(0, 0),
		wxSize(150, 250),
		wxNO_BORDER | wxWANTS_CHARS);
	grid->CreateGrid(50, 20);
	return grid;
}

wxTreeCtrl *Frame::CreateTreeCtrl()
{
	wxTreeCtrl *tree = new wxTreeCtrl(this, 10009,
		wxPoint(0, 0), wxSize(190, 250),
		wxTR_DEFAULT_STYLE | wxNO_BORDER);

	tree->SetWindowStyle(wxTR_HIDE_ROOT);

	wxImageList* imglist = new wxImageList(16, 16, true, 2);
	imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
	imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16)));
	tree->AssignImageList(imglist);

	wxTreeItemId root = tree->AddRoot("NoRoot", 0);

	wxTreeItemId id = tree->AppendItem(root, wxT("Block ciphers"), 0);
	tree->AppendItem(id, wxT("AES"), 1);
	tree->AppendItem(id, wxT("Blowfish"), 1);
	tree->AppendItem(id, wxT("Twofish"), 1);
	tree->AppendItem(id, wxT("Serpent"), 1);
	tree->AppendItem(id, wxT("Camellia"), 1);
	tree->AppendItem(id, wxT("CAST128"), 1);
	tree->AppendItem(id, wxT("CAST256"), 1);
	tree->AppendItem(id, wxT("GOST"), 1);
	tree->AppendItem(id, wxT("MARS"), 1);
	tree->AppendItem(id, wxT("RC2"), 1);
	tree->AppendItem(id, wxT("RC4"), 1);
	tree->AppendItem(id, wxT("RC5"), 1);
	tree->AppendItem(id, wxT("RC6"), 1);
	tree->AppendItem(id, wxT("DES"), 1);
	tree->AppendItem(id, wxT("DESX"), 1);
	tree->AppendItem(id, wxT("3DES"), 1);
	tree->AppendItem(id, wxT("3kDES"), 1);
	tree->AppendItem(id, wxT("SAFER-K"), 1);
	tree->AppendItem(id, wxT("SAFER-SK"), 1);
	tree->AppendItem(id, wxT("SEED"), 1);
	tree->AppendItem(id, wxT("SHACAL-2"), 1);
	tree->AppendItem(id, wxT("SHARK"), 1);
	tree->AppendItem(id, wxT("Skipjack"), 1);
	tree->AppendItem(id, wxT("Square"), 1);
	tree->AppendItem(id, wxT("TEA"), 1);
	tree->AppendItem(id, wxT("IDEA"), 1);
	tree->AppendItem(id, wxT("BTEA"), 1);
	tree->AppendItem(id, wxT("XTEA"), 1);
	tree->AppendItem(id, wxT("NULL"), 1);

	id = tree->AppendItem(root, wxT("Stream ciphers"), 0);
	tree->AppendItem(id, wxT("ChaCha8"), 1);
	tree->AppendItem(id, wxT("ChaCha12"), 1);
	tree->AppendItem(id, wxT("ChaCha20"), 1);
	tree->AppendItem(id, wxT("Salsa20"), 1);
	tree->AppendItem(id, wxT("XSalsa20"), 1);
	tree->AppendItem(id, wxT("Panama-LE"), 1);
	tree->AppendItem(id, wxT("Panama-BE"), 1);
	tree->AppendItem(id, wxT("Seal-LE"), 1);
	tree->AppendItem(id, wxT("Seal-BE"), 1);
	tree->AppendItem(id, wxT("WAKE"), 1);

	id = tree->AppendItem(root, wxT("Hash functions"), 0);
	tree->AppendItem(id, wxT("BLAKE2s"), 1);
	tree->AppendItem(id, wxT("BLAKE2b"), 1);
	tree->AppendItem(id, wxT("Keccak"), 1);
	tree->AppendItem(id, wxT("SHA1"), 1);
	tree->AppendItem(id, wxT("SHA224"), 1);
	tree->AppendItem(id, wxT("SHA256"), 1);
	tree->AppendItem(id, wxT("SHA384"), 1);
	tree->AppendItem(id, wxT("SHA512"), 1);
	tree->AppendItem(id, wxT("SHA3"), 1);
	tree->AppendItem(id, wxT("Tiger"), 1);
	tree->AppendItem(id, wxT("Whirlpool"), 1);
	tree->AppendItem(id, wxT("RIPEMD128"), 1);
	tree->AppendItem(id, wxT("RIPEMD160"), 1);
	tree->AppendItem(id, wxT("RIPEMD256"), 1);
	tree->AppendItem(id, wxT("RIPEMD320"), 1);
	tree->AppendItem(id, wxT("MD2"), 1);
	tree->AppendItem(id, wxT("MD4"), 1);
	tree->AppendItem(id, wxT("MD5"), 1);

	id = tree->AppendItem(root, wxT("Public key schemes"), 0);
	tree->AppendItem(id, wxT("DLIES"), 1);
	tree->AppendItem(id, wxT("ECIES"), 1);
	tree->AppendItem(id, wxT("LUCES"), 1);
	tree->AppendItem(id, wxT("RSAES"), 1);
	tree->AppendItem(id, wxT("RabinES"), 1);
	tree->AppendItem(id, wxT("LUC_IES"), 1);

	id = tree->AppendItem(root, wxT("Signature schemes"), 0);
	tree->AppendItem(id, wxT("DSA2"), 1);
	tree->AppendItem(id, wxT("GDSA"), 1);
	tree->AppendItem(id, wxT("ECDSA"), 1);
	tree->AppendItem(id, wxT("NR"), 1);
	tree->AppendItem(id, wxT("ECNR"), 1);
	tree->AppendItem(id, wxT("LUCSS"), 1);
	tree->AppendItem(id, wxT("RSASS"), 1);
	tree->AppendItem(id, wxT("RSASS-ISO"), 1);
	tree->AppendItem(id, wxT("RabinSS"), 1);
	tree->AppendItem(id, wxT("RWSS"), 1);
	tree->AppendItem(id, wxT("ESIGN"), 1);

	id = tree->AppendItem(root, wxT("Key agreement"), 0);
	tree->AppendItem(id, wxT("DH"), 1);
	tree->AppendItem(id, wxT("DH2"), 1);
	tree->AppendItem(id, wxT("ECDH"), 1);
	tree->AppendItem(id, wxT("MQV"), 1);
	tree->AppendItem(id, wxT("ECMQV"), 1);
	tree->AppendItem(id, wxT("HMQV"), 1);
	tree->AppendItem(id, wxT("ECHMQV"), 1);
	tree->AppendItem(id, wxT("FHMQV"), 1);
	tree->AppendItem(id, wxT("ECFHMQV"), 1);
	tree->AppendItem(id, wxT("XTR_DH "), 1);

	id = tree->AppendItem(root, wxT("Universal checksums"), 0);
	tree->AppendItem(id, wxT("CRC32"), 1);
	tree->AppendItem(id, wxT("Adler32"), 1);

	id = tree->AppendItem(root, wxT("Message authentication"), 0);
	tree->AppendItem(id, wxT("VMAC"), 1);
	tree->AppendItem(id, wxT("HMAC"), 1);
	tree->AppendItem(id, wxT("CBC-MAC"), 1);
	tree->AppendItem(id, wxT("CMAC"), 1);
	tree->AppendItem(id, wxT("DMAC"), 1);
	tree->AppendItem(id, wxT("TTMAC"), 1);
	tree->AppendItem(id, wxT("GMAC"), 1);
	tree->AppendItem(id, wxT("BLAKE2b"), 1);
	tree->AppendItem(id, wxT("BLAKE2s"), 1);
	tree->AppendItem(id, wxT("Poly1305"), 1);

	id = tree->AppendItem(root, wxT("Key derivation"), 0);
	tree->AppendItem(id, wxT("HKDF"), 1);
	tree->AppendItem(id, wxT("PBKDF"), 1);
	tree->AppendItem(id, wxT("PBKDF1"), 1);
	tree->AppendItem(id, wxT("PBKDF2-HMAC"), 1);

	id = tree->AppendItem(root, wxT("Random generators"), 0);
	tree->AppendItem(id, wxT("LCRNG"), 1);
	tree->AppendItem(id, wxT("MT19937"), 1);
	tree->AppendItem(id, wxT("MT19937ar"), 1);
	tree->AppendItem(id, wxT("RDRAND"), 1);
	tree->AppendItem(id, wxT("RDSEED"), 1);

	return tree;
}

wxPropertyGridManager *Frame::CreatePropCtrl()
{
	wxPropertyGridManager *pgman = new wxPropertyGridManager(this, wxID_ANY, wxPoint(0, 0), wxSize(250, 250),
		wxPG_DESCRIPTION |
		wxPG_SPLITTER_AUTO_CENTER);

	wxPropertyGridPage *page =  pgman->AddPage();
	page->Append(new wxPropertyCategory(wxT("General"), wxPG_LABEL));
	page->Append(new wxStringProperty(wxT("Application"), wxPG_LABEL, GetTitle()));
	page->Append(new wxStringProperty(wxT("OS"), wxPG_LABEL, ::wxGetOsDescription()));
	page->Append(new wxStringProperty(wxT("Hostname"), wxPG_LABEL, ::wxGetHostName()));
	page->Append(new wxStringProperty(wxT("User Id"), wxPG_LABEL, ::wxGetUserId()));
	page->Append(new wxDirProperty(wxT("User Home"), wxPG_LABEL, ::wxGetUserHome()));
	page->Append(new wxStringProperty(wxT("User Name"), wxPG_LABEL, ::wxGetUserName()));

	page->SetPropertyHelpString(wxT("Application"), wxT("Application name."));
	page->SetPropertyHelpString(wxT("OS"), wxT("Operating system name."));
	page->SetPropertyHelpString(wxT("Hostname"), wxT("Current hostname."));
	page->SetPropertyHelpString(wxT("User Id"), wxT("Operating system user ID."));
	page->SetPropertyHelpString(wxT("User Home"), wxT("User home directory."));
	page->SetPropertyHelpString(wxT("User Name"), wxT("Username executing process."));

	page->DisableProperty(wxT("Application"));
	page->DisableProperty(wxT("OS"));
	page->DisableProperty(wxT("Hostname"));
	page->DisableProperty(wxT("User Id"));
	page->DisableProperty(wxT("User Home"));
	page->DisableProperty(wxT("User Name"));

	// page->Append(new wxPropertyCategory(wxT("Properties"), wxPG_LABEL));

	return pgman;
}

wxSizeReportCtrl *Frame::CreateSizeReportCtrl(int width, int height)
{
	wxSizeReportCtrl *ctrl = new wxSizeReportCtrl(this, wxID_ANY,
		wxDefaultPosition,
		wxSize(width, height), &m_mgr);
	return ctrl;
}

wxHtmlWindow *Frame::CreateHTMLCtrl(wxWindow *parent)
{
	if (!parent)
		parent = this;

	wxHtmlWindow *ctrl = new wxHtmlWindow(parent, wxID_ANY,
		wxDefaultPosition,
		wxSize(400, 300));
	ctrl->SetPage(GetIntroText());
	return ctrl;
}

wxAuiNotebook *Frame::CreateNotebook()
{
	// create the notebook off-window to avoid flicker
	wxSize client_size = GetClientSize();

	wxAuiNotebook *ctrl = new wxAuiNotebook(this, wxID_ANY,
		wxPoint(client_size.x, client_size.y),
		wxSize(430, 200),
		m_notebook_style);

	//wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	wxBitmap page_bmp = wxNullBitmap;

	ctrl->AddPage(CreateHTMLCtrl(ctrl), wxT("Start page"), false, page_bmp);
	ctrl->SetPageToolTip(0, "Welcome to Cryptox");

	// Panel
	wxPanel *panel = new wxPanel(ctrl, wxID_ANY);
	wxFlexGridSizer *flex = new wxFlexGridSizer(4, 2, 0, 0);
	flex->AddGrowableRow(0);
	flex->AddGrowableRow(3);
	flex->AddGrowableCol(1);
	flex->Add(5, 5);
	flex->Add(5, 5);
	flex->Add(new wxStaticText(panel, wxID_ANY, wxT("wxTextCtrl:")), 0, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(100, -1)), 1, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(new wxStaticText(panel, wxID_ANY, wxT("wxSpinCtrl:")), 0, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(new wxSpinCtrl(panel, wxID_ANY, wxT("5"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 5, 50, 5), 0, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(5, 5);
	flex->Add(5, 5);
	panel->SetSizer(flex);
	ctrl->AddPage(panel, wxT("Panel"), false, page_bmp);

	// Dataview
	wxDataViewCtrl *dataview = new wxDataViewCtrl(ctrl, wxID_ANY);
	SecretListModel *listmodel = new SecretListModel;
	dataview->AssociateModel(listmodel);
	dataview->AppendTextColumn("editable string",
		SecretListModel::Col_EditableText,
		wxDATAVIEW_CELL_EDITABLE,
		wxCOL_WIDTH_AUTOSIZE,
		wxALIGN_NOT,
		wxDATAVIEW_COL_SORTABLE);
	dataview->AppendIconTextColumn("icon",
		SecretListModel::Col_IconText,
		wxDATAVIEW_CELL_EDITABLE,
		wxCOL_WIDTH_AUTOSIZE,
		wxALIGN_NOT,
		wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_SORTABLE);

	dataview->AppendDateColumn("date", SecretListModel::Col_Date);
	wxDataViewColumn *attributes = new wxDataViewColumn("attributes", new wxDataViewTextRenderer,
			SecretListModel::Col_TextWithAttr,
			wxCOL_WIDTH_AUTOSIZE,
			wxALIGN_RIGHT,
			wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	dataview->AppendColumn(attributes);
	ctrl->AddPage(dataview, wxT("List"), false, page_bmp);
	
	// Console
	wxTextCtrl *console = new wxTextCtrl(ctrl, 10010, wxT("Type 'help' to get started\n\n>> "), wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE |
		wxNO_BORDER |
		wxTE_PROCESS_ENTER);
	console->SetInsertionPointEnd();
	ctrl->AddPage(console, wxT("Console"), false, page_bmp);

	return ctrl;
}

void Frame::OnItemMenu(wxTreeEvent& event)
{
	wxTreeCtrl *tree = static_cast<wxTreeCtrl *>(event.GetEventObject());

	// Skip parents
	wxTreeItemId itemId = event.GetItem();
	if (tree->ItemHasChildren(itemId))
		return;

	wxPoint pt = tree->ClientToScreen(event.GetPoint());
	pt = ScreenToClient(pt);

	// Build popup menu
	wxMenu menu;
	menu.Append(ID_RandomGeneratorWindow, wxT("&Run"));
	menu.AppendSeparator();
	menu.Append(wxID_ANY, wxT("&Use as template"));
	menu.Append(wxID_ANY, wxT("&Attack database"));

	PopupMenu(&menu, pt);
	event.Skip();
}

void Frame::OnConsoleEnter(wxCommandEvent& event)
{
	wxDateTime timestamp(wxDateTime::Now());

	wxTextCtrl *console = static_cast<wxTextCtrl *>(event.GetEventObject());
	console->AppendText(wxT("\nUnrecognized command\n\n>> "));
	m_output->AppendText("\n[");
	m_output->AppendText(timestamp.FormatISOTime());
	m_output->AppendText("] The console caused an error");
	event.Skip();
}

wxString Frame::GetIntroText()
{
	static const char *text =
		"<html><body>"
		"<h3>Welcome to Cryptox</h3>"
		"<br/><b>Overview</b><br/>"
		"<p>wxAUI is an Advanced User Interface library for the wxWidgets toolkit "
		"that allows developers to create high-quality, cross-platform user "
		"interfaces quickly and easily.</p>"
		"<p><b>Features</b></p>"
		"<p>With wxAUI, developers can create application frameworks with:</p>"
		"<ul>"
		"<li>Native, dockable floating frames</li>"
		"<li>Perspective saving and loading</li>"
		"<li>Native toolbars incorporating real-time, &quot;spring-loaded&quot; dragging</li>"
		"<li>Customizable floating/docking behaviour</li>"
		"<li>Completely customizable look-and-feel</li>"
		"<li>Optional transparent window effects (while dragging or docking)</li>"
		"<li>Splittable notebook control</li>"
		"</ul>"
		"<p><b>What's new in 0.9.4?</b></p>"
		"<p>wxAUI 0.9.4, which is bundled with wxWidgets, adds the following features:"
		"<ul>"
		"<li>New wxAuiToolBar class, a toolbar control which integrates more "
		"cleanly with wxAuiFrameManager.</li>"
		"<li>Lots of bug fixes</li>"
		"</ul>"
		"<p><b>What's new in 0.9.3?</b></p>"
		"<p>wxAUI 0.9.3, which is now bundled with wxWidgets, adds the following features:"
		"<ul>"
		"<li>New wxAuiNotebook class, a dynamic splittable notebook control</li>"
		"<li>New wxAuiMDI* classes, a tab-based MDI and drop-in replacement for classic MDI</li>"
		"<li>Maximize/Restore buttons implemented</li>"
		"<li>Better hinting with wxGTK</li>"
		"<li>Class rename.  'wxAui' is now the standard class prefix for all wxAUI classes</li>"
		"<li>Lots of bug fixes</li>"
		"</ul>"
		"<p><b>What's new in 0.9.2?</b></p>"
		"<p>The following features/fixes have been added since the last version of wxAUI:</p>"
		"<ul>"
		"<li>Support for wxMac</li>"
		"<li>Updates for wxWidgets 2.6.3</li>"
		"<li>Fix to pass more unused events through</li>"
		"<li>Fix to allow floating windows to receive idle events</li>"
		"<li>Fix for minimizing/maximizing problem with transparent hint pane</li>"
		"<li>Fix to not paint empty hint rectangles</li>"
		"<li>Fix for 64-bit compilation</li>"
		"</ul>"
		"<p><b>What changed in 0.9.1?</b></p>"
		"<p>The following features/fixes were added in wxAUI 0.9.1:</p>"
		"<ul>"
		"<li>Support for MDI frames</li>"
		"<li>Gradient captions option</li>"
		"<li>Active/Inactive panes option</li>"
		"<li>Fix for screen artifacts/paint problems</li>"
		"<li>Fix for hiding/showing floated window problem</li>"
		"<li>Fix for floating pane sizing problem</li>"
		"<li>Fix for drop position problem when dragging around center pane margins</li>"
		"<li>LF-only text file formatting for source code</li>"
		"</ul>"
		"<p>See README.txt for more information.</p>"
		"</body></html>";

	return wxString::FromAscii(text);
}

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_MENU(Frame::ID_CreateTree, Frame::OnCreateTree)
	EVT_MENU(Frame::ID_CreateGrid, Frame::OnCreateGrid)
	EVT_MENU(Frame::ID_CreateText, Frame::OnCreateText)
	EVT_MENU(Frame::ID_CreateHTML, Frame::OnCreateHTML)
	EVT_MENU(Frame::ID_CreateSizeReport, Frame::OnCreateSizeReport)
	EVT_MENU(Frame::ID_CreateNotebook, Frame::OnCreateNotebook)
	EVT_MENU(Frame::ID_CreatePerspective, Frame::OnCreatePerspective)
	EVT_MENU(Frame::ID_CopyPerspectiveCode, Frame::OnCopyPerspectiveCode)
	EVT_MENU(ID_AllowFloating, Frame::OnManagerFlag)
	EVT_MENU(ID_TransparentHint, Frame::OnManagerFlag)
	EVT_MENU(ID_VenetianBlindsHint, Frame::OnManagerFlag)
	EVT_MENU(ID_RectangleHint, Frame::OnManagerFlag)
	EVT_MENU(ID_NoHint, Frame::OnManagerFlag)
	EVT_MENU(ID_HintFade, Frame::OnManagerFlag)
	EVT_MENU(ID_NoVenetianFade, Frame::OnManagerFlag)
	EVT_MENU(ID_TransparentDrag, Frame::OnManagerFlag)
	EVT_MENU(ID_LiveUpdate, Frame::OnManagerFlag)
	EVT_MENU(ID_AllowActivePane, Frame::OnManagerFlag)
	EVT_MENU(ID_NoGradient, Frame::OnGradient)
	EVT_MENU(ID_VerticalGradient, Frame::OnGradient)
	EVT_MENU(ID_HorizontalGradient, Frame::OnGradient)
	EVT_MENU(ID_AllowToolbarResizing, Frame::OnToolbarResizing)
	EVT_MENU(ID_Settings, Frame::OnSettings)
	EVT_MENU(ID_RandomGeneratorWindow, Frame::OnMenuPrimitiveRun)
	EVT_MENU(ID_CustomizeToolbar, Frame::OnCustomizeToolbar)
	EVT_MENU(wxID_EXIT, Frame::OnExit)
	EVT_MENU(wxID_ABOUT, Frame::OnAbout)
	EVT_UPDATE_UI(ID_AllowFloating, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_TransparentHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_VenetianBlindsHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_RectangleHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NoHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_HintFade, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NoVenetianFade, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_TransparentDrag, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_LiveUpdate, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NoGradient, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_VerticalGradient, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_HorizontalGradient, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_AllowToolbarResizing, Frame::OnUpdateUI)
	EVT_MENU_RANGE(Frame::ID_FirstPerspective, Frame::ID_FirstPerspective + 1000,
		Frame::OnRestorePerspective)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_DropDownToolbarItem, Frame::OnDropDownToolbarItem)
	EVT_AUI_PANE_CLOSE(Frame::OnPaneClose)
	EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, Frame::OnAllowNotebookDnD)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, Frame::OnNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, Frame::OnNotebookPageClosed)
	EVT_TREE_ITEM_MENU(10009, Frame::OnItemMenu)
	EVT_TREE_ITEM_ACTIVATED(10009, Frame::OnTreeDoubleClick)
	EVT_TEXT_ENTER(10010, Frame::OnConsoleEnter)
wxEND_EVENT_TABLE()
