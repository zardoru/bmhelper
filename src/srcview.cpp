
#include "srcview.h"
#include "frame.h"


BEGIN_EVENT_TABLE(SourceView, wxWindow)
	EVT_SIZE(SourceView::OnSize)
END_EVENT_TABLE()


SourceView::SourceView(FrameWindow *_frame, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: frame(_frame), wxWindow(parent, id, pos, size, style, name), midi(0)
{
	SetWindowStyle(wxBORDER_NONE);

	midi = new MidiView(this, 1);
	Connect(1, wxEVT_SET_STATUS_TEXT, wxCommandEventHandler(SourceView::OnSetStatusText));

	_Size();
}


SourceView::~SourceView(){
}

void SourceView::OnSize(wxSizeEvent &WXUNUSED(event)){
	_Size();
}

void SourceView::_Size(){
	wxSize size = GetClientSize();
	if (midi) midi->SetSize(0, 0, size.x, size.y);
}


void SourceView::ProjectChanged(){
	if (frame->project){
		midi->SetMidiData(&frame->project->GetSource());
	}else{
		midi->SetMidiData(0);
	}
}


void SourceView::OnSetStatusText(wxCommandEvent &event){
	frame->UpdateStatus(event.GetString());
}


