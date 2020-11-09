
#pragma once

#include "common.h"
#include "gui.h"
#include "midi_data.h"
#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <vector>


class MidiView : public wxScrolledWindow{

	static const int zoom_base = 65536;

	wxBitmap bmp;
	wxMemoryDC mdc;
	wxPen pen_measure, pen_beat, pen_sub;

	MidiData *data;
	int max_time{};
	int min_nn{}, max_nn{};
	int rmin_nn{}, rmax_nn{};

	//void _Repaint();
	void Repaint(const wxRegion *rgn=0);
	void DrawNote(const wxRect &rc, int velocity);

    wxSize zoom_rate;
	wxSize cls_old;
	wxRect GetNoteRect(const MidiNoteEvent &note);
	int NoteFromPoint(const wxPoint &pt);

public:
	MidiView(wxWindow *parent, wxWindowID id);
	~MidiView();

	void SetMidiData(MidiData *_data);


	void OnDraw(wxDC &dc);

	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnMouseMotion(wxMouseEvent &event);
	void OnMouseWheel(wxMouseEvent &event);
public:
};


