
#include "srcview.h"
#include "frame.h"
#include "divview.h"

#include <wx/dialog.h>


enum{
	ID_NewDivision = 0x0101
};

enum{
	ID_ListBox = 0x0801
};


BEGIN_EVENT_TABLE(DivisionsView, wxWindow)
	EVT_COMMAND(ID_NewDivision, wxEVT_COMMAND_BUTTON_CLICKED, DivisionsView::OnNewDivision)
	EVT_SIZE(DivisionsView::OnSize)
END_EVENT_TABLE()


DivisionsView::DivisionsView(FrameWindow *_frame, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxWindow(parent, id, pos, size, style, name), frame(_frame),
new_div(0), divisions(0), editor(0)
{
	SetWindowStyle(wxBORDER_NONE | wxCLIP_CHILDREN);

	new_div = new wxButton(this, ID_NewDivision, _("New division..."));
	divisions = new wxListBox(this, ID_ListBox);
	Connect(ID_ListBox, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(DivisionsView::OnSelectDivision));
	editor = new DivisionEditor(frame, this, 0);

	DivisionChanged();
	_Size();
}


DivisionsView::~DivisionsView(){
}

void DivisionsView::OnSize(wxSizeEvent &WXUNUSED(event)){
	_Size();
}


void DivisionsView::OnSelectDivision(wxCommandEvent &WXUNUSED(event)){
	DivisionChanged();
}

void DivisionsView::_Size(){
	wxSize size = GetClientSize();
	if (size.x > 100){
		new_div->Show(true);
		divisions->Show(true);
		new_div->SetSize(0, 0, 100, 45);
		divisions->SetSize(0, 45, 100, size.y-45);
		editor->SetSize(100, 0, size.x-100, size.y);
	}else{
		new_div->Show(false);
		divisions->Show(false);
		editor->SetSize(0, 0, size.x, size.y);
	}
}


void DivisionsView::ProjectChanged(){
	if (frame->project){
		divisions->Clear();
		for (size_t i=0; i<frame->project->GetDivisionsCount(); i++){
			divisions->Append(frame->project->GetDivision(i).get_name());
		}
		if (frame->project->GetDivisionsCount()){
			divisions->SetSelection(0);
		}
		frame->m_div_new->Enable(true);
		new_div->Enable(true);
	}else{
		divisions->Clear();
		frame->m_div_new->Enable(false);
		new_div->Enable(false);
	}
	DivisionChanged();
}


void DivisionsView::OnDeleteDivision(wxMenuEvent &WXUNUSED(event)){
	if (!frame->project) return;
	int selection = divisions->GetSelection();
	if (selection == wxNOT_FOUND) return;
	Division *div = &frame->project->GetDivision(selection);
	wxString msg = wxString::Format(_("Are you sure you want to delete %s?"), div->get_name());
	int r = wxMessageBox( msg, app_name, wxYES_NO | wxICON_EXCLAMATION, this );
	if (r != wxYES) return;
	divisions->Delete(selection);
	frame->project->EraseDivision(selection);
	if (selection < (int)divisions->GetCount()){
		divisions->SetSelection(selection);
	}else if (selection > 0){
		divisions->SetSelection(selection-1);
	}
	DivisionChanged();
}

void DivisionsView::DivisionChanged(){
	int selection = divisions->GetSelection();
	if (selection == wxNOT_FOUND){
		editor->SetDivision(0);
		frame->UpdateDivision(0);
		frame->m_div_rename->Enable(false);
		frame->m_div_delete->Enable(false);
		frame->m_div_smfout->Enable(false);
		frame->m_div_divcopy->Enable(false);
		frame->m_div_defout->Enable(false);
		frame->m_div_seqcopy->Enable(false);
	}else{
	    auto & div = frame->project->GetDivision(selection);
	    divisions->SetString(selection, div.get_name());
		editor->SetDivision(&div);
		frame->UpdateDivision(&div);
        frame->m_div_rename->Enable(true);
		frame->m_div_delete->Enable(true);
		frame->m_div_smfout->Enable(true);
		frame->m_div_divcopy->Enable(true);
		frame->m_div_defout->Enable(true);
		frame->m_div_seqcopy->Enable(true);
	}
}


//======================================



class DivisionSettingDialog : public wxDialog{
	enum{
		ID_NameLabel = 1,
		ID_NameInput = 2,
		ID_MidiBox   = 30,
		ID_ScpyCheck = 29,
		ID_MhmgLabel = 32,
		ID_MhmgInput = 33,
		ID_MintLabel = 3,
		ID_MintInput = 4,
		ID_SortLabel = 5,
		ID_SortCombo = 6,
		ID_ThrsLabel = 20,
		ID_TgatLabel = 7,
		ID_TgatInput = 8,
		ID_TvelLabel = 9,
		ID_TvelInput = 10,
		ID_DefnBox   = 31,
//		ID_DfzzLabel = 11,
		ID_DfzzCheck = 12,
		ID_DfmlCheck = 21,
		ID_DfstLabel = 13,
		ID_DfstInput = 14,
		ID_DfmtLabel = 22,
		ID_DfmtInput = 23
	};
	wxStaticText	*name_label;
	wxTextCtrl		*name_input;
	wxStaticBox		*midi_box;
	wxCheckBox		*scpy_check;
	wxStaticText	*mhmg_label;
	wxTextCtrl		*mhmg_input;
	wxStaticText	*mint_label;
	wxTextCtrl		*mint_input;
	wxStaticText	*sort_label;
	wxComboBox		*sort_combo;
	wxStaticText	*thrs_label;
	wxStaticText	*tgat_label;
	wxTextCtrl		*tgat_input;
	wxStaticText	*tvel_label;
	wxTextCtrl		*tvel_input;
	wxStaticBox		*defn_box;
//	wxStaticText	*dfzz_label;
	wxCheckBox		*dfzz_check;
	wxCheckBox		*dfml_check;
	wxStaticText	*dfst_label;
	wxTextCtrl		*dfst_input;
	wxStaticText	*dfmt_label;
	wxTextCtrl		*dfmt_input;
	wxButton *button_ok, *button_cancel;
	int quantize;
	static const int _lm=15, _tm=5, _lw=25, _lb=30, _fc=150, _sc=130, _bw=5, _cw=25;
public:
	DivisionSettingDialog(wxWindow *owner, DivisionSetting setting, int _quantize) :
	  wxDialog(owner, -1, _("Division settings"), wxDefaultPosition, wxSize(_lm*2+_fc+_sc+_bw*2+10, _tm*2+_lw*15+_lb+_bw+_cw+10)), quantize(_quantize)
	{
		static wxString sort_choices[] = {
			_("None"),
			_("nn/gate/vel"),
			_("nn/vel/gate"),
			_("gate/nn/vel"),
			_("gate/vel/nn"),
			_("vel/nn/gate"),
			_("vel/gate/nn"),
		};
		name_label = new wxStaticText(this, ID_NameLabel, _("Name (&N):"), wxPoint(_lm,_tm+3), wxSize(_fc,15));
		name_input = new wxTextCtrl(this, ID_NameInput, setting.name, wxPoint(_lm+_fc, _tm), wxSize(_sc, 18));
        midi_box = new wxStaticBox(this, ID_MidiBox, _("MIDI settings"), wxPoint(5,_tm+_lw+3), wxSize(_fc+_sc+23,_lw*8+3));
        scpy_check = new wxCheckBox(this, ID_ScpyCheck, _("Leave MIDI unaltered (&C)"), wxPoint(_lm, _tm+_lw*2), wxSize(_fc+_sc,18)); scpy_check->SetValue(setting.src_copy);
        mhmg_label = new wxStaticText(this, ID_MhmgLabel, _("Leading silence (&H):"), wxPoint(_lm,_tm+_lw*3+3), wxSize(_fc,15));
        mhmg_input = new wxTextCtrl(this, ID_MhmgInput, wxString::Format(_("%f"), (double)setting.head_margin/quantize), wxPoint(_lm+_fc, _tm+_lw*3), wxSize(_sc, 18));
        mint_label = new wxStaticText(this, ID_MintLabel, _("Min. interval gap (&I):"), wxPoint(_lm,_tm+_lw*4+3), wxSize(_fc,15));
        mint_input = new wxTextCtrl(this, ID_MintInput, wxString::Format(_("%f"), (double)setting.min_interval/quantize), wxPoint(_lm+_fc, _tm+_lw*4), wxSize(_sc, 18));
        sort_label = new wxStaticText(this, ID_SortLabel, _("Sorting method (&S):"), wxPoint(_lm,_tm+_lw*5+3), wxSize(_fc,15));
        sort_combo = new wxComboBox(this, ID_SortCombo, sort_choices[(int)setting.sort_type], wxPoint(_lm+_fc, _tm+_lw*5), wxSize(_sc, 18), 7, sort_choices, wxCB_DROPDOWN | wxCB_READONLY);
        thrs_label = new wxStaticText(this, ID_ThrsLabel, _("Minimum difference in:"), wxPoint(_lm,_tm+_lw*6+3), wxSize(_fc+_sc,15));
        tgat_label = new wxStaticText(this, ID_TgatLabel, _(" Length (&G):"), wxPoint(_lm,_tm+_lw*7+3), wxSize(_fc,15));
        tgat_input = new wxTextCtrl(this, ID_TgatInput, wxString::Format(_("%f"), (double)setting.gate_threshold/quantize), wxPoint(_lm+_fc, _tm+_lw*7), wxSize(_sc, 18));
        tvel_label = new wxStaticText(this, ID_TvelLabel, _(" Velocity (&V):"), wxPoint(_lm,_tm+_lw*8+3), wxSize(_fc,15));
        tvel_input = new wxTextCtrl(this, ID_TvelInput, wxString::Format(_("%zu"), setting.velocity_threshold), wxPoint(_lm+_fc, _tm+_lw*8), wxSize(_sc, 18));
        defn_box = new wxStaticBox(this, ID_DefnBox, _("Definition settings"), wxPoint(5,_tm+_lw*10+3), wxSize(_fc+_sc+23,_lw*4+3));
//		dfzz_label = new wxStaticText(this, ID_DfzzLabel, _("ZZ定義を有効にする(&Z):"), wxPoint(_lm,_tm+_lw*11+3), wxSize(_fc,15));
        dfzz_check = new wxCheckBox(this, ID_DfzzCheck, _("Use base-36 range(00-ZZ) (&Z)"), wxPoint(_lm, _tm+_lw*11), wxSize((_fc+_sc)/2,18)); dfzz_check->SetValue(setting.zz_definition);
        dfml_check = new wxCheckBox(this, ID_DfmlCheck, _("Use overloading (&M)"), wxPoint(_lm+(_fc+_sc)/2,_tm+_lw*11), wxSize((_fc+_sc)/2,18)); dfml_check->SetValue(setting.ml_definition);
        dfst_label = new wxStaticText(this, ID_DfstLabel, _("Starting BMS Index (&D):"), wxPoint(_lm,_tm+_lw*12+3), wxSize(_fc,15));
        dfst_input = new wxTextCtrl(this, ID_DfstInput, setting.start_definition.to_string(), wxPoint(_lm+_fc, _tm+_lw*12), wxSize(_sc, 18));
        dfmt_label = new wxStaticText(this, ID_DfmtLabel, _("Overloading Interval (&L):"), wxPoint(_lm,_tm+_lw*13+3), wxSize(_fc,15));
        dfmt_input = new wxTextCtrl(this, ID_DfmtInput, wxString::Format(_("%f"), (double)setting.ml_threshold/quantize), wxPoint(_lm+_fc, _tm+_lw*13), wxSize(_sc, 18));
        button_ok = new wxButton(this, wxID_OK, _("OK"), wxPoint(_lm,_tm+_lw*15), wxSize((_fc+_sc-_lm)/2, 28));
        button_cancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(_lm+(_fc+_sc+_lm)/2,_tm+_lw*15), wxSize((_fc+_sc+_lm)/2, 28));
		button_ok->SetDefault();
		SetEscapeId(wxID_CANCEL);
	}
	void GetSetting(DivisionSetting &setting){
		double db;
		long lb;
		setting.name = name_input->GetValue();
		setting.src_copy = scpy_check->GetValue();
		if (mhmg_input->GetValue().ToDouble(&db)) setting.head_margin = (int)(db*quantize);
		if (mint_input->GetValue().ToDouble(&db)) setting.min_interval = (int)(db*quantize);
		setting.sort_type = (DivisionSetting::SortType)sort_combo->GetSelection();
		if (tgat_input->GetValue().ToDouble(&db)) setting.gate_threshold = (int)(db*quantize);
		if (tvel_input->GetValue().ToLong(&lb)) setting.velocity_threshold = lb;
		setting.zz_definition = dfzz_check->GetValue();
		setting.ml_definition = dfml_check->GetValue();
		setting.start_definition.from_string(dfst_input->GetValue());
		if (dfmt_input->GetValue().ToDouble(&db)) setting.ml_threshold = (int)(db*quantize);
	}
	void OnScpyCheck(wxCommandEvent &WXUNUSED(ev)){
		bool enabled = !scpy_check->GetValue();
		mint_input->Enable(enabled);
		sort_combo->Enable(enabled);
		tgat_input->Enable(enabled);
		tvel_input->Enable(enabled);
	}
	void OnOK(wxCommandEvent &WXUNUSED(ev)){
		wxString s = dfst_input->GetValue();
		ZZNumber zz;
		bool zz_enabled = dfzz_check->GetValue();
		if (s.length() != 2 || !zz.from_string(s) || (!zz_enabled && !zz.in_ff())){
			MessageBeep(0); // windows
			return;
		}
		this->EndDialog(wxID_OK);
	}
	DECLARE_EVENT_TABLE()
};


wxBEGIN_EVENT_TABLE(DivisionSettingDialog, wxDialog)
	EVT_CHECKBOX(DivisionSettingDialog::ID_ScpyCheck, DivisionSettingDialog::OnScpyCheck)
	EVT_COMMAND(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, DivisionSettingDialog::OnOK)
wxEND_EVENT_TABLE()



void DivisionsView::_NewDivision(){
	if (!frame->project) return;
	int quantize = frame->project->GetSource().get_quantize();
	DivisionSetting setting;

	wxFileName fn(frame->project->GetSource().source_filename);
	auto fn_no_ext = fn.GetName();

	setting.name = fn_no_ext;
	setting.src_copy = false;
	setting.head_margin = 0;
	setting.min_interval = quantize*1;
	setting.sort_type = DivisionSetting::SORT_NN_GATE_V;
	setting.gate_threshold = quantize/8;
	setting.velocity_threshold = 5;
	setting.zz_definition = true;
	setting.ml_definition = true;
	setting.start_definition = 1;
	setting.ml_threshold = quantize/2;
	DivisionSettingDialog *dialog = new DivisionSettingDialog(frame, setting, quantize);
	int ret = dialog->ShowModal();
	if (ret != wxID_OK){
		dialog->Destroy();
		return;
	}
	dialog->GetSetting(setting);
	dialog->Destroy();
	size_t i = frame->project->CreateDivision(setting);
	divisions->SetSelection(divisions->Append(setting.name));
	DivisionChanged();
}

void DivisionsView::OnNewDivision(wxCommandEvent &WXUNUSED(event)){
	_NewDivision();
}


void DivisionsView::OnNewDivision(wxMenuEvent &WXUNUSED(event)){
	_NewDivision();
}

void DivisionsView::OnDivRename(wxMenuEvent &event) {
    if (editor->OnDivRename(event)) {
        frame->project->SetChangeFlag();
        DivisionChanged();
    }
}




