
#pragma once

#include "common.h"
#include "gui.h"
#include "project.h"
#include <wx/splitter.h>

class FrameWindow;
class MidiView;
class DefinitionView;


class DivisionEditor : public wxWindow{

	FrameWindow *frame;
	MidiView *midi;
	DefinitionView *defview;
	wxButton *smf_output, *wos_clipboard;
	wxButton *def_clipboard, *bms_clipboard;
	wxButton *open_splitter;
	wxButton *def_transpose_up, *def_transpose_down, *def_transpose_to;
	Division *division;

	wxSplitterWindow* horizontal_splitter;

    void _SmfOut();
	void _DivCopy();
	void DefOut();
	void _SeqCopy();
	bool DivRename();

public:
	DivisionEditor(FrameWindow *_frame, wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxCLIP_CHILDREN,
		const wxString& name = _("DivisionsEditor"));

	void SetDivision(Division *_div);
	DECLARE_EVENT_TABLE()
public:
	void OnSize(wxSizeEvent &event);
	void OnSmfOut(wxCommandEvent &event){ _SmfOut(); }
	void OnDivCopy(wxCommandEvent &event){ _DivCopy(); }
	void OnDefOut(wxCommandEvent &event){ DefOut(); }
	void OnSeqCopy(wxCommandEvent &event){ _SeqCopy(); }
	void OnSmfOut(wxMenuEvent &event){ _SmfOut(); }
	void OnDivCopy(wxMenuEvent &event){ _DivCopy(); }
	bool OnDivRegenerate(wxMenuEvent &event);
	void OnDefOut(wxMenuEvent &event){ DefOut(); }
    bool OnDivRename(wxMenuEvent &event) { return DivRename(); }
	void OnSeqCopy(wxMenuEvent &event){ _SeqCopy(); }
	void OnOpenAudioSplitter(wxCommandEvent &event);
	void OnSetStatusText(wxCommandEvent &event);
	void OnDefTransposeUp(wxCommandEvent &event);
	void OnDefTransposeDown(wxCommandEvent &event);
	void OnDefTransposeTo(wxCommandEvent &event);
};



class DivisionsView : public wxWindow{

	friend class FrameWindow;
	FrameWindow *frame;

	wxButton *new_div;
	wxListBox *divisions;
	DivisionEditor *editor;
	wxSplitterWindow *splitter;

	void _Size();
	void _NewDivision();

public:
	DivisionsView(FrameWindow *_frame, wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxCLIP_CHILDREN,
		const wxString& name = _("DivisionsView"));
	~DivisionsView();

	void OnSize(wxSizeEvent &event);
	void OnNewDivision(wxCommandEvent &event);
	void OnNewDivision(wxMenuEvent &event);
	void OnDeleteDivision(wxMenuEvent &event);
    void OnDivRename(wxMenuEvent &event);
    void OnDivRegenerate(wxMenuEvent &event);
	void OnSelectDivision(wxCommandEvent &event);
	void ProjectChanged();
	void DivisionChanged();

	DECLARE_EVENT_TABLE()
};


