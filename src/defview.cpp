
#include "defview.h"
#include <wx/dcclient.h>
#include <wx/settings.h>
#include <wx/sizer.h>


BEGIN_EVENT_TABLE(DefinitionView, wxWindow)
	EVT_PAINT(DefinitionView::OnPaint)
	EVT_SIZE(DefinitionView::OnSize)
	EVT_SCROLLWIN(DefinitionView::OnScroll)
END_EVENT_TABLE()


static const int item_width = 30;



DefinitionView::DefinitionView(wxWindow *parent, wxWindowID id)
: wxWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxHSCROLL | wxALWAYS_SHOW_SB), division(0)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	//SetWindowStyle(wxBORDER_THEME | wxHSCROLL | wxALWAYS_SHOW_SB);
	//SetScrollRate(1,0);
	SetVirtualSize(0,0);
	SetScrollbar(wxHORIZONTAL, 0, 0, 0);
}

DefinitionView::~DefinitionView(){
}

void DefinitionView::SetDivision(Division *_division){
	division = _division;
	ReloadDivision();
}

void DefinitionView::ReloadDivision(){
	items.clear();
	if (!division){
		SetVirtualSize(0,0);
		Refresh();
		return;
	}
	for (size_t i=0; i<division->definitions_count(); i++){
		DefinitionViewItem item;
		item.zz_num_string = division->definition(i).zz.to_string();
		item.div_num_string = wxString::Format(_("%d"), division->definition(i).div);
		item.selected = false;
		items.push_back(item);
	}
	//SetVirtualSize(GetClientSize().x+1,0);		// スクロールバー描画のバグ回避的なアレ
	SetVirtualSize(items.size()*item_width, 0);
	{
		SetScrollbar(wxHORIZONTAL, 0, GetClientSize().x/item_width, items.size());
	}
	Refresh();
}

void DefinitionView::OnSize(wxSizeEvent &WXUNUSED(e)){
	// スクロールバー描画がバグる事があるけど対処できないので仕方ないね
	int clx = GetClientSize().x;
	int pos = GetScrollPos(wxHORIZONTAL);
	if (pos<0) pos=0;
	if (pos>(int)items.size()-clx/item_width) pos=(int)items.size()-clx/item_width;
	SetScrollbar(wxHORIZONTAL, pos, clx/item_width, items.size());
	Refresh();
}

void DefinitionView::OnScroll(wxScrollWinEvent &e){
	int clx = GetClientSize().x;
	int pos = GetScrollPos(wxHORIZONTAL);
	wxEventType type = e.GetEventType();
	if (type == wxEVT_SCROLLWIN_TOP)				pos = 0;
	else if (type == wxEVT_SCROLLWIN_BOTTOM)		pos = (items.size()-1)*item_width;
	else if (type == wxEVT_SCROLLWIN_LINEUP)		pos--;
	else if (type == wxEVT_SCROLLWIN_LINEDOWN)		pos++;
	else if (type == wxEVT_SCROLLWIN_PAGEUP)		pos -= clx/item_width;
	else if (type == wxEVT_SCROLLWIN_PAGEDOWN)		pos += clx/item_width;
	else if (type == wxEVT_SCROLLWIN_THUMBTRACK)	pos = e.GetPosition();
	if (pos<0) pos=0;
	if (pos>(int)items.size()-clx/item_width) pos=(int)items.size()-clx/item_width;
	SetScrollPos(wxHORIZONTAL, pos);
	Refresh();
}

void DefinitionView::OnPaint(wxPaintEvent &WXUNUSED(e)){
	static const int q=9;
	wxPaintDC dc(this);
	wxSize size = GetClientSize();
	if (!division){
		wxColour gray = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
		dc.GradientFillLinear(wxRect(0,0,size.x,size.y), gray, gray);
		return;
	}
	int sx = GetScrollPos(wxHORIZONTAL)*item_width;
	wxColour back = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	wxColour back2((back.Red()*q+127)/(q+1), (back.Green()*q+127)/(q+1), (back.Blue()*q+127)/(q+1));
	dc.SetBackgroundMode(wxTRANSPARENT);
	dc.SetTextForeground(wxSYS_COLOUR_WINDOWTEXT);
	dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	for (size_t i=0; i<items.size(); i++){
		if (i%2){
			dc.GradientFillLinear(wxRect(i*item_width-sx, 0, item_width, size.y), back2, back2, wxSOUTH);
		}
		dc.DrawText(items[i].zz_num_string, i*item_width+3-sx, 3);
		dc.DrawText(items[i].div_num_string, i*item_width+3-sx, 20);
	}
}




