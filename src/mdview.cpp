
#include "mdview.h"


wxBEGIN_EVENT_TABLE(MidiView, wxScrolledWindow)
	//EVT_PAINT(MidiView::OnPaint)
	EVT_SIZE(MidiView::OnSize)
	EVT_MOTION(MidiView::OnMouseMotion)
	EVT_MOUSEWHEEL(MidiView::OnMouseWheel)
wxEND_EVENT_TABLE()


MidiView::MidiView(wxWindow *parent, wxWindowID id)
: wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxHSCROLL | wxVSCROLL | wxALWAYS_SHOW_SB),
data(nullptr),
bmp(wxGetDisplaySize().x, wxGetDisplaySize().y), mdc(bmp),
pen_measure(wxColor(144,144,144), 2), pen_beat(wxColor(108,108,108)), pen_sub(wxColor(80,80,80)),
cls_old(GetClientSize())
{
	SetBackgroundColour(wxColor(64,64,64));
	//DoPrepareDC(mdc);
}


MidiView::~MidiView()= default;


void MidiView::SetMidiData(MidiData *_data){
	if (!(data = _data)){
		Refresh();
		return;
	}
	max_time = 0;
	rmin_nn = 127;
	rmax_nn = 0;
	for (auto i=data->notes_begin(); i!=data->notes_end(); i++){
		if (i->position + i->gate > max_time) max_time = i->position + i->gate;
		if (i->note_num < rmin_nn) rmin_nn = i->note_num;
		if (i->note_num > rmax_nn) rmax_nn = i->note_num;
	}
	if (rmin_nn > rmax_nn) rmin_nn = rmax_nn;
	//min_nn = max(0, rmin_nn-12);
	//max_nn = min(127, rmax_nn+12);
	min_nn = 0;
	max_nn = 127;
	if (max_time == 0) max_time = 1;

	// 拡大率を初期化
	wxSize cls = cls_old;
	zoom_rate.x = cls.x * zoom_base / max_time;
	if (zoom_rate.x > zoom_base*2) zoom_rate.x = zoom_base*2;
	if (zoom_rate.x < zoom_base/4) zoom_rate.x = zoom_base/4;
	zoom_rate.y = zoom_base*8;
	wxSize vs;
	vs.x = zoom_rate.x * max_time / zoom_base;
	vs.y = zoom_rate.y * (max_nn - min_nn + 1) / zoom_base;
	//SetVirtualSize(vs);
	//SetScrollRate(1, 1);
	//SetScrollPos(wxHORIZONTAL, 0);
	int ypos = (max_nn-rmax_nn)*zoom_rate.y/zoom_base + ((rmax_nn-rmin_nn+1)*zoom_rate.y/zoom_base - cls.y)/2;
	//SetScrollPos(wxVERTICAL, ypos);
	SetScrollbars(1, 1, vs.x, vs.y, 0, ypos);

	Refresh();
}


/*void MidiView::OnPaint(wxPaintEvent &WXUNUSED(e)){
	int px = GetScrollPos(wxHORIZONTAL);
	int py = GetScrollPos(wxVERTICAL);
	wxPaintDC dc(this);
	for (wxRegionIterator upd(GetUpdateRegion()); upd; upd++){
		wxRect rect = upd.GetRect();
		dc.Blit(rect.x-px, rect.y-py, rect.width, rect.height, &mdc, rect.x, rect.y);
	}
}*/

void MidiView::OnDraw(wxDC &dc){
	int px = GetScrollPos(wxHORIZONTAL);
	int py = GetScrollPos(wxVERTICAL);
	// wxSize size = GetClientSize();
	wxRegion rgn = GetUpdateRegion();
	Repaint(&rgn);
	for (wxRegionIterator upd(rgn); upd; ++upd){
		wxRect rect = upd.GetRect();
		dc.Blit(rect.x+px, rect.y+py, rect.width, rect.height, &mdc, rect.x, rect.y);
	}
}

void MidiView::OnSize(wxSizeEvent &WXUNUSED(e)){
    // 縦方向は中央の位置を保存
    int px = GetScrollPos(wxHORIZONTAL);
    int py = GetScrollPos(wxVERTICAL);
    wxSize cls = GetClientSize();
    // double cy = py + cls.y/2;
    int z = (cls_old.y - cls.y)/2;
    py += z;
    wxSize vs = GetVirtualSize();
    SetScrollbars(1,1,vs.x,vs.y,px,py);
    cls_old = cls;
    Refresh(false);
}

void MidiView::Repaint(const wxRegion *rgn){
	int px = GetScrollPos(wxHORIZONTAL);
	// int py = GetScrollPos(wxVERTICAL);
	wxSize size = GetVirtualSize();
	wxRect rc = rgn? rgn->GetBox() : wxRect(0,0,size.x, size.y);
	if (rgn) mdc.SetDeviceClippingRegion(*rgn);
	if (!data){
		mdc.GradientFillLinear(rc, wxColor(192,192,192), wxColor(192,192,192), wxSOUTH);
		if (rgn) mdc.DestroyClippingRegion();
		return;
	}
	mdc.GradientFillLinear(rc, wxColor(64,64,64), wxColor(64,64,64), wxSOUTH);
	int line_iv;
	mdc.SetPen(pen_sub);
	line_iv = zoom_rate.x*data->get_quantize()/4;
	if (line_iv > 10*zoom_base){
		for (int i=(rc.x+px)*zoom_base/line_iv; i*line_iv/zoom_base<rc.x+rc.width+px; i++){
			int x = i*line_iv/zoom_base - px;
			mdc.DrawLine(x, rc.y, x, rc.y+rc.height);
		}
	}
	mdc.SetPen(pen_beat);
	line_iv = zoom_rate.x*data->get_quantize();
	if (line_iv > 10*zoom_base){
		for (int i=(rc.x+px)*zoom_base/line_iv; i*line_iv/zoom_base<rc.x+rc.width+px; i++){
			int x = i*line_iv/zoom_base - px;
			mdc.DrawLine(x, rc.y, x, rc.y+rc.height);
		}
	}
	mdc.SetPen(pen_measure);
	line_iv = zoom_rate.x*data->get_quantize()*4;	// 拍子によって要変更
	if (line_iv > 10*zoom_base){
		for (int i=(rc.x+px)*zoom_base/line_iv; i*line_iv/zoom_base<rc.x+rc.width+px; i++){
			int x = i*line_iv/zoom_base - px;
			mdc.DrawLine(x, rc.y, x, rc.y+rc.height);
		}
	}
	for (auto i=data->notes_begin(); i!=data->notes_end(); i++){
        DrawNote(GetNoteRect(*i), i->velocity);
	}
	if (rgn) mdc.DestroyClippingRegion();
}

/*void MidiView::_Repaint(){
	wxSize size = GetVirtualSize();
	wxSize disp_size = wxGetDisplaySize();
	if (size.x < disp_size.x) size.x = disp_size.x;
	if (size.y < disp_size.y) size.y = disp_size.y;
	bmp.Create(size.x, size.y);
	wxRect rc(0, 0, size.x, size.y);
	if (!data){
		mdc.GradientFillLinear(rc, wxColor(192,192,192), wxColor(192,192,192), wxSOUTH);
		return;
	}
	mdc.GradientFillLinear(rc, wxColor(64,64,64), wxColor(64,64,64), wxSOUTH);
	mdc.SetPen(pen_beat);
	if (size.x*data->get_quantize()/max_time > 10){
		for (int i=data->get_quantize(); i<max_time; i+=data->get_quantize()){
			int x = zoom_rate.x*i/zoom_base;
			mdc.DrawLine(x, 0, x, size.y);
		}
	}
	mdc.SetPen(pen_measure);
	for (int i=data->get_quantize()*4; i<max_time; i+=data->get_quantize()*4){
		int x = zoom_rate.x*i/zoom_base;
		mdc.DrawLine(x, 0, x, size.y);
	}
	for (MidiData::MidiNotesLane::iterator i=data->notes_begin(); i!=data->notes_end(); i++){
		DrawNote(GetNoteRect(*i), i->velocity);
	}

}*/

wxRect MidiView::GetNoteRect(const MidiNoteEvent &note){
	wxRect rc;
	rc.x = zoom_rate.x*note.position/zoom_base;
	rc.y = zoom_rate.y*(max_nn-note.note_num)/zoom_base;
	rc.width = zoom_rate.x*(note.position+note.gate)/zoom_base - rc.x;
	rc.height = zoom_rate.y*(max_nn-note.note_num+1)/zoom_base - rc.y;
	rc.x -= GetScrollPos(wxHORIZONTAL);
	rc.y -= GetScrollPos(wxVERTICAL);
	return rc;
}

void MidiView::DrawNote(const wxRect &rc, int velocity){
	int c1 = 64 + (160-64)*velocity/128;
	int c2 = 64 + (256-64)*velocity/128;
	wxRect rci(rc.x+1, rc.y, rc.width-2, rc.height);
	wxRect rcii(rc.x+1, rc.y+1, rc.width-2, rc.height-2);
	mdc.GradientFillLinear(rc, wxColor(255,255,255), wxColor(0,0,0), wxEAST);
	mdc.GradientFillLinear(rci, wxColor(255,255,255), wxColor(0,0,0), wxSOUTH);
	mdc.GradientFillLinear(rcii, wxColor(c1,c2,c1), wxColor(c1,c2,c1), wxSOUTH);
}


void MidiView::OnMouseWheel(wxMouseEvent &e){
	if (!data) return;
	wxSize cls = GetClientSize();
	wxSize vts = GetVirtualSize();
	int px = GetScrollPos(wxHORIZONTAL);
	int py = GetScrollPos(wxVERTICAL);
	if (e.ControlDown()){
		// zoom
		if (e.ShiftDown()){
			// horz
			int z_org = zoom_rate.x;
			if (e.GetWheelRotation()>0){
				zoom_rate.x = zoom_rate.x * 4 / 3;
				if (zoom_rate.x > zoom_base*2) zoom_rate.x = zoom_base*2;
			}else{
				zoom_rate.x = zoom_rate.x * 3 / 4;
				if (zoom_rate.x < zoom_base/4) zoom_rate.x = zoom_base/4;
			}
			px = (int)((double)(px+cls.x/2.0)*(double)zoom_rate.x/(double)z_org) - cls.x/2;
		}else{
			// vert
			int z_org = zoom_rate.y;
			if (e.GetWheelRotation()>0){
				zoom_rate.y = zoom_rate.y * 4 / 3;
				if (zoom_rate.y > zoom_base*32) zoom_rate.y = zoom_base*32;
			}else{
				zoom_rate.y = zoom_rate.y * 3 / 4;
				if (zoom_rate.y < zoom_base) zoom_rate.y = zoom_base;
			}
			py = (int)((double)(py+cls.y/2.0)*(double)zoom_rate.y/(double)z_org) - cls.y/2;
		}
		wxSize vs;
		vs.x = zoom_rate.x * max_time / zoom_base;
		vs.y = zoom_rate.y * (max_nn - min_nn + 1) / zoom_base;
		//SetVirtualSize(vs);
		SetScrollbars(1,1,vs.x,vs.y,px,py);
		this->Refresh();
	}else{
		// scroll
		if (e.ShiftDown()){
			// horz
			if (cls.x < vts.x){
				int amount = e.GetWheelRotation()>0 ? -zoom_rate.x*data->get_quantize()/zoom_base : zoom_rate.x*data->get_quantize()/zoom_base;
				Scroll(GetScrollPos(wxHORIZONTAL)+amount, -1);
			}
		}else{
			// vert
			if (cls.y < vts.y){
				int amount = e.GetWheelRotation()>0 ? -zoom_rate.y/zoom_base : zoom_rate.y/zoom_base;
				Scroll(-1, GetScrollPos(wxVERTICAL)+amount);
			}
		}
	}
}


static wxString get_note_num_string(int note_num){
	int octv = note_num/12;
	int key = note_num%12;
	static const wxString kn[] = {
		_("C "), _("C#"), _("D "), _("D#"), _("E "), _("F "), _("F#"),
		_("G "), _("G#"), _("A "), _("A#"), _("B "), 
	};
	return wxString::Format(_("%s %d"), kn[key], octv);
}


void MidiView::OnMouseMotion(wxMouseEvent &event){
	wxString text(_(""));
	if (data){
		wxPoint p = event.GetPosition();
		int i=NoteFromPoint(p);
		if (i>=0){
			MidiNoteEvent &note = data->notes(i);
			text = wxString::Format(_("Note#%d ( %s ) gate:%d/%d vel:%d"), i,
				get_note_num_string(note.note_num), note.gate, data->get_quantize(),
				note.velocity);
		}
	}
	wxCommandEvent ev(wxEVT_SET_STATUS_TEXT, GetId());
	ev.SetEventObject(this);
	ev.SetString(text);
	GetEventHandler()->ProcessEvent(ev);
}

int MidiView::NoteFromPoint(const wxPoint &pt){
	for (size_t i=0; i<data->notes_count(); i++){
		if (GetNoteRect(data->notes(i)).Contains(pt)){
			return (signed)i;
		}
	}
	return -1;
}


