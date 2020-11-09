

#include "frame.h"



wxDEFINE_EVENT(wxEVT_SET_STATUS_TEXT, wxCommandEvent);

enum {
	ID_New		= 0x0101,
	ID_Open		= 0x0102,
	ID_Save		= 0x0103,
	ID_SaveAs	= 0x0104,
	ID_Close	= 0x0105,
	ID_Quit		= 0x010f,
	ID_DivNew		= 0x0201,
	ID_DivDelete	= 0x0202,
	ID_DivSmfOut	= 0x0203,
	ID_DivDivCopy	= 0x0204,
	ID_DivDefOut	= 0x0205,
	ID_DivSeqCopy	= 0x0206,
	ID_DivRename    = 0x0207,
	ID_Help		= 0x0901,
	ID_About	= 0x0903
};


wxBEGIN_EVENT_TABLE(FrameWindow, wxFrame)
	EVT_MENU(ID_New, FrameWindow::OnNewProject)
	EVT_MENU(ID_Open, FrameWindow::OnOpenProject)
	EVT_MENU(ID_Save, FrameWindow::OnSaveProject)
	EVT_MENU(ID_SaveAs, FrameWindow::OnSaveProjectAs)
	EVT_MENU(ID_Close, FrameWindow::OnCloseProject)
	EVT_MENU(ID_Quit, FrameWindow::OnQuit)
	EVT_MENU(ID_Help, FrameWindow::OnHelp)
	EVT_MENU(ID_About, FrameWindow::OnAbout)
	EVT_CLOSE(FrameWindow::OnClose)
	EVT_DROP_FILES(FrameWindow::OnDropFiles)
	EVT_SIZE(FrameWindow::OnSize)
wxEND_EVENT_TABLE()



FrameWindow::FrameWindow(const wxPoint &pos, const wxSize &size)
: wxFrame(NULL, -1, app_name, pos, size),
menu_bar(new wxMenuBar), status_bar(CreateStatusBar()),
m_file(new wxMenu), m_div(new wxMenu), m_help(new wxMenu),
m_file_new(new wxMenuItem(m_file, ID_New, _("New project (&N)...\tCtrl+N"))),
m_file_open(new wxMenuItem(m_file, ID_Open, _("Open project (&O)...\tCtrl+O"))),
m_file_save(new wxMenuItem(m_file, ID_Save, _("Save project (&S)\tCtrl+S"))),
m_file_save_as(new wxMenuItem(m_file, ID_SaveAs, _("Save project as (&A)...\tCtrl+Shift+S"))),
m_file_close(new wxMenuItem(m_file, ID_Close, _("Close project (&C)"))),
m_file_quit(new wxMenuItem(m_file, ID_Quit, _("Quit (&X)\tCtrl+Q"))),
m_div_new(new wxMenuItem(m_div, ID_DivNew, _("New division (&N)...\tF5"))),
m_div_rename(new wxMenuItem(m_div, ID_DivRename, _("Rename division (&R)...\tF2"))),
m_div_delete(new wxMenuItem(m_div, ID_DivDelete, _("Delete division (&D)"))),
m_div_smfout(new wxMenuItem(m_div, ID_DivSmfOut, _("Write modified MIDI File (&M)..."))),
m_div_divcopy(new wxMenuItem(m_div, ID_DivDivCopy, _("Copy division (&W)"))),
m_div_defout(new wxMenuItem(m_div, ID_DivDefOut, _("Definition information (&D)..."))),
m_div_seqcopy(new wxMenuItem(m_div, ID_DivSeqCopy, _("Copy BMS sequence to clipboard (&S)"))),
m_help_help(new wxMenuItem(m_help, ID_Help, _("Help (&H)...\tF1"))),
m_help_about(new wxMenuItem(m_help, ID_About, _("About (&I)..."))),
splitter(0), src(0), div(0),
project(0)
{
	// init gui
	wxIcon ico("BMHICON", wxBITMAP_TYPE_ICO_RESOURCE);
	SetIcon(ico);

	wxAcceleratorEntry acs[7];
	acs[0].Set(wxACCEL_CTRL, (int)'N', ID_New);
	acs[1].Set(wxACCEL_CTRL, (int)'O', ID_Open);
	acs[2].Set(wxACCEL_CTRL, (int)'S', ID_Save);
	acs[3].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'S', ID_SaveAs);
	acs[4].Set(wxACCEL_CTRL, (int)'Q', ID_Quit);
	acs[5].Set(wxACCEL_NORMAL, WXK_F5, ID_DivNew);
	acs[6].Set(wxACCEL_NORMAL, WXK_F2, ID_Help);
	wxAcceleratorTable accel(7, acs);
	SetAcceleratorTable(accel);

	m_file->Append(m_file_new);
	m_file->Append(m_file_open);
	m_file->Append(m_file_save);
	m_file->Append(m_file_save_as);
	m_file->Append(m_file_close);
	m_file->AppendSeparator();
	m_file->Append(m_file_quit);
	menu_bar->Append(m_file, _("File (&F)"));
	m_div->Append(m_div_new);
	m_div->Append(m_div_rename);
	m_div->Append(m_div_delete);
	m_div->AppendSeparator();
	m_div->Append(m_div_smfout);
	m_div->Append(m_div_divcopy);
	m_div->Append(m_div_defout);
	m_div->Append(m_div_seqcopy);
	menu_bar->Append(m_div, _("Divisions (&D)"));
	m_help->Append(m_help_help);
	m_help->Append(m_help_about);
	menu_bar->Append(m_help, _("Help (&H)"));
	SetMenuBar(menu_bar);

	status_bar->SetFieldsCount(3);
	int sw[3] = { -1, 180, 120 };
	status_bar->SetStatusWidths(3, sw);

	splitter = new wxSplitterWindow(this, 0, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
	src = new SourceView(this, splitter, 0);
	div = new DivisionsView(this, splitter, 0);
	splitter->SplitHorizontally(src, div, 0);
	splitter->SetSashGravity(0.5);
	splitter->SetMinimumPaneSize(80);

	Connect(ID_DivNew, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionsView::OnNewDivision), 0, div);
	Connect(ID_DivDelete, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionsView::OnDeleteDivision), 0, div);
	Connect(ID_DivSmfOut, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionEditor::OnSmfOut), 0, div->editor);
	Connect(ID_DivDivCopy, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionEditor::OnDivCopy), 0, div->editor);
	Connect(ID_DivDefOut, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionEditor::OnDefOut), 0, div->editor);
	Connect(ID_DivSeqCopy, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionEditor::OnSeqCopy), 0, div->editor);
    Connect(ID_DivRename, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(DivisionsView::OnDivRename), 0, div);

	DragAcceptFiles(true);
}

FrameWindow::~FrameWindow(){
	SetProject(0);
}

void FrameWindow::OpenFiles(int nFiles, std::vector<wxString> FileNames){
	for (int i=0; i<nFiles; i++){
		wxString filename(FileNames[i]);
		try{
			SetProject(new Project(filename));
		}catch(...){
			project = nullptr;
			wxMessageBox(wxString(_("Couldn't open file:\r\n")) + filename, app_name, wxICON_ERROR | wxOK, this);
			continue;
		}
		break;
	}
	ProjectChanged();
}

void FrameWindow::SetProject(Project *_project){
	if (project){
		project->SetObserver(0);
		delete project;
	}
	if (project = _project){
		project->SetObserver(this);
	}
}

void FrameWindow::OnNewProject(wxCommandEvent &WXUNUSED(event)){
	if (project && !_AskCloseProject()) return;
	wxString filename = wxFileSelector(
	        _("Please select source MIDI file"),
	        wxString(),
	        wxString(),
	        _("mid"),
	        midi_file_filter,
	        wxFD_OPEN | wxFD_FILE_MUST_EXIST,
	        this
    );

	if (filename.empty()) return;
	try{
		SetProject(new Project(filename));
	}catch(...){
		wxMessageBox(_("Couldn't open file"), app_name, wxICON_ERROR | wxOK, this);
		return;
	}
	ProjectChanged();
}

void FrameWindow::OnOpenProject(wxCommandEvent &WXUNUSED(event)){
	if (project && !_AskCloseProject()) return;
	wxString filename = wxFileSelector(_("Open project file"), wxString(), wxString(), project_file_ext, project_file_filter, wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
	if (filename.empty()) return;
	try{
		SetProject(new Project(filename));
	}catch(...){
		wxMessageBox(_("Couldn't open file"), app_name, wxICON_ERROR | wxOK, this);
		return;
	}
	ProjectChanged();
}

void FrameWindow::OnSaveProject(wxCommandEvent &WXUNUSED(event)){
	if (!project) return;
	if (!project->Titled()){
		_SaveProjectAs();
		return;
	}
	project->Save();
}

void FrameWindow::OnSaveProjectAs(wxCommandEvent &WXUNUSED(event)){
	if (!project) return;
	_SaveProjectAs();
}

void FrameWindow::OnCloseProject(wxCommandEvent &WXUNUSED(event)){
	if (!project) return;
	if (!_AskCloseProject()) return;
	SetProject(0);
	ProjectChanged();
}


void FrameWindow::OnQuit(wxCommandEvent &WXUNUSED(event)){
	if (project){
		if (!_AskCloseProject()) return;
		SetProject(0);
	}
	Destroy();
}

void FrameWindow::OnClose(wxCloseEvent &event){
	if (event.CanVeto() && project){
		if (!_AskCloseProject()) return;
		SetProject(nullptr);
	}
	Destroy();
}

void FrameWindow::OnDropFiles(wxDropFilesEvent &event){
	Raise();
	if (project && !_AskCloseProject()) return;
	for (int i=0; i<event.GetNumberOfFiles(); i++){
		try{
			SetProject(new Project(event.GetFiles()[i]));
		}catch(...){
			wxMessageBox(wxString(_("Couldn't open file:\r\n")) + event.GetFiles()[i], app_name, wxICON_ERROR | wxOK, this);
			continue;
		}
		break;
	}
	ProjectChanged();
}

void FrameWindow::OnSize(wxSizeEvent &WXUNUSED(event)){
	_Size();
}

void FrameWindow::OnHelp(wxCommandEvent &WXUNUSED(event)){
    /* TODO: fix this */
	/* char fn[MAX_PATH];
	GetModuleFileName(NULL, fn, MAX_PATH);
	wsprintf(PathFindFileName(fn), "%s", "readme.html");
	ShellExecute(NULL, NULL, fn, NULL, NULL, SW_SHOWDEFAULT); */
}

void FrameWindow::OnAbout(wxCommandEvent &WXUNUSED(event)){
	auto text = wxString::Format("%s (%s)\n by exclusion\n forked by zardoru", app_name, app_version);
	wxMessageBox(text, app_name, wxOK | wxICON_INFORMATION, this);
}

// ---

bool FrameWindow::_SaveProjectAs(){
	if (!project) return false;
	wxString filename = wxFileSelector(_("Save project as"), wxString(), project->GetFileName(), project_file_ext, project_file_filter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
	if (filename.empty()) return false;
	return project->SaveAs(filename);
}

bool FrameWindow::_AskCloseProject(){
	if (!project) return false;
	if (project->Changed()){
		int res = wxMessageBox(_("The project has been modified. Do you want to save?"), app_name, wxYES_NO | wxCANCEL, this);
		if (res == wxCANCEL) return false;
		if (res == wxYES){
			if (project->Titled()){
				project->Save();
			}else{
				if (!_SaveProjectAs()) return false;
			}
		}
	}
	return true;
}

void FrameWindow::ProjectChanged(){
	ProjectChangeFlagChanged();
	if (project){
		status_bar->SetStatusText(wxString::Format(_("source: %zun"), project->GetSource().notes_count()), 2);
		status_bar->SetStatusText(_(""), 1);
		src->ProjectChanged();
		div->ProjectChanged();
		splitter->Show(true);
		m_file_save->Enable(true);
		m_file_save_as->Enable(true);
		m_file_close->Enable(true);
	}else{
		status_bar->SetStatusText(_(""), 2);
		status_bar->SetStatusText(_(""), 1);
		src->ProjectChanged();
		div->ProjectChanged();
		splitter->Show(false);
		m_file_save->Enable(false);
		m_file_save_as->Enable(false);
		m_file_close->Enable(false);
	}
}

void FrameWindow::ProjectFileNameChanged(){
	ProjectChangeFlagChanged();
}

void FrameWindow::ProjectChangeFlagChanged(){
	wxString caption = project?
		(project->Titled()?
			(project->Changed()?
				project->GetFileTitle() + wxString(_("* - ")) + app_name
			: project->GetFileTitle() + wxString(_(" - ")) + app_name)
		: wxString(_("New project* - ")) + app_name)
	: app_name;
	SetTitle(caption);
}

void FrameWindow::_Size(){
	wxSize size = GetClientSize();
	if (splitter) splitter->SetSize(0, 0, size.x, size.y);
}

void FrameWindow::UpdateDivision(Division *division){
	if (!status_bar) return;
	if (division){
		status_bar->SetStatusText(wxString::Format(_("%s: %zun/%zud"), division->get_name(), division->notes_count(), division->definitions_count()), 1);
	}else{
		status_bar->SetStatusText(_(""), 1);
	}
}

void FrameWindow::UpdateStatus(const wxString &text){
	if (!status_bar) return;
	status_bar->SetStatusText(text, 0);
}



