
#pragma once


#include "division.h"
#include <wx/scrolwin.h>
#include <vector>


struct DefinitionViewItem{
	wxString zz_num_string;
	wxString div_num_string;
	bool selected;
};



class DefinitionView : public wxWindow{

	Division *division;
	std::vector<DefinitionViewItem> items;

	void _Repaint();
	void _DrawNote(const wxRect &rc, int velocity);
	void _Size();


public:
	DefinitionView(wxWindow *parent, wxWindowID id);
	~DefinitionView();

	void SetDivision(Division *_division);
	void ReloadDivision();

	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnScroll(wxScrollWinEvent &event);
public:
};




