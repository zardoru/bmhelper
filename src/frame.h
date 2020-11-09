
#pragma once

#include "common.h"
#include "project.h"
#include "srcview.h"
#include "divview.h"
#include <wx/splitter.h>



class FrameWindow : public wxFrame, public ProjectObserver{

	friend class SourceView;
	friend class DivisionsView;
	friend class DivisionEditor;

	wxMenuBar *menu_bar;
	wxStatusBar *status_bar;
	wxMenu *m_file, *m_div, *m_help;
	wxMenuItem *m_file_new, *m_file_open, *m_file_save, *m_file_save_as, *m_file_close, *m_file_quit;
	wxMenuItem *m_div_new, *m_div_delete, *m_div_smfout, *m_div_divcopy, *m_div_defout, *m_div_seqcopy;
	wxMenuItem *m_help_help, *m_help_about;
	wxSplitterWindow *splitter;
	SourceView *src;
	DivisionsView *div;

	Project *project;

	bool _AskCloseProject();
	bool _SaveProjectAs();
	void _Size();
	void ProjectChanged();
	void UpdateDivision(Division *division);
	void UpdateStatus(const wxString &text);
	void SetProject(Project *_project);

public:

	FrameWindow(const wxPoint &pos, const wxSize &size);
	~FrameWindow();

	void OpenFiles(int nFiles, std::vector<wxString> FileNames);
	virtual void ProjectFileNameChanged();
	virtual void ProjectChangeFlagChanged();

	// commands
	void OnNewProject(wxCommandEvent &event);
	void OnOpenProject(wxCommandEvent &event);
	void OnSaveProject(wxCommandEvent &event);
	void OnSaveProjectAs(wxCommandEvent &event);
	void OnCloseProject(wxCommandEvent &event);
	void OnQuit(wxCommandEvent &event);
	void OnHelp(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnClose(wxCloseEvent &event);
	void OnDropFiles(wxDropFilesEvent &event);
	void OnSize(wxSizeEvent &event);

	wxDECLARE_EVENT_TABLE();

};



