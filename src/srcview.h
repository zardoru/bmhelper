
#pragma once

#include "common.h"
#include "gui.h"
#include "project.h"
#include "mdview.h"


class FrameWindow;


class SourceView : public wxWindow{

	FrameWindow *frame;

	MidiView *midi;

	void _Size();

public:
	SourceView(FrameWindow *_frame, wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxCLIP_CHILDREN,
		const wxString& name = _("SourceView"));
	~SourceView();

	void ProjectChanged();

	DECLARE_EVENT_TABLE()
	void OnSize(wxSizeEvent &event);
	void OnSetStatusText(wxCommandEvent &event);
};
