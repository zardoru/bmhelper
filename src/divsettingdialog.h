#ifndef BMHELPER_DIVSETTINGDIALOG_H
#define BMHELPER_DIVSETTINGDIALOG_H

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
    DivisionSettingDialog(wxWindow *owner, DivisionSetting setting, int _quantize);
    void GetSetting(DivisionSetting &setting);
    void OnScpyCheck(wxCommandEvent &WXUNUSED(ev));
    void OnOK(wxCommandEvent &WXUNUSED(ev));
    wxDECLARE_EVENT_TABLE();
};

#endif //BMHELPER_DIVSETTINGDIALOG_H
