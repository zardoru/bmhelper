
#include "divview.h"
#include "mdview.h"
#include "defview.h"
#include "frame.h"
#include "AudioSplitter.h"
#include <wx/clipbrd.h>
#include <wx/dialog.h>

enum {
    ID_SmfOutput,
    ID_WosClipboard,
    ID_DefClipboard,
    ID_BmsClipboard,
    ID_DefTransposeUp,
    ID_DefTransposeDown,
    ID_DefTransposeTo,
    ID_MidiView,
    ID_OpenSplitter
};


wxBEGIN_EVENT_TABLE(DivisionEditor, wxWindow)
    EVT_SIZE(DivisionEditor::OnSize)
    EVT_BUTTON(ID_SmfOutput, DivisionEditor::OnSmfOut)
    EVT_BUTTON(ID_WosClipboard, DivisionEditor::OnDivCopy)
    EVT_BUTTON(ID_DefClipboard, DivisionEditor::OnDefOut)
    EVT_BUTTON(ID_BmsClipboard, DivisionEditor::OnSeqCopy)
    EVT_BUTTON(ID_OpenSplitter, DivisionEditor::OnOpenAudioSplitter)
    EVT_COMMAND(ID_MidiView, wxEVT_SET_STATUS_TEXT, DivisionEditor::OnSetStatusText)
    EVT_BUTTON(ID_DefTransposeUp, DivisionEditor::OnDefTransposeUp)
    EVT_BUTTON(ID_DefTransposeDown, DivisionEditor::OnDefTransposeDown)
    EVT_BUTTON(ID_DefTransposeTo, DivisionEditor::OnDefTransposeTo)
wxEND_EVENT_TABLE()


DivisionEditor::DivisionEditor(FrameWindow *_frame, wxWindow *parent, wxWindowID id,
                               const wxPoint &pos, const wxSize &size, long style, const wxString &name)
        : wxWindow(parent, id, pos, size, style, name), frame(_frame),
          midi(0), defview(0), smf_output(0), wos_clipboard(0), def_clipboard(0), bms_clipboard(0),
          division(0) {
    SetWindowStyle(wxBORDER_NONE | wxCLIP_CHILDREN);
    //                 Horizontal Splitter
    //                           v
    /*                  MIDI     | BUTN1
     * Vert. Splitter > =========| BUTN2
     *                  TRANSPOSE| BUTNn
     * */

    horizontal_splitter = new wxSplitterWindow(this);

    /* Left panel */
    auto left_panel = new wxPanel(horizontal_splitter);
    auto left_panel_sizer = new wxBoxSizer(wxVERTICAL);
    left_panel->SetSizer(left_panel_sizer);

    auto vertical_splitter = new wxSplitterWindow(left_panel);

    midi = new MidiView(vertical_splitter, ID_MidiView);

    /* transpose panel */
    auto transpose_panel = new wxPanel(vertical_splitter);
    auto transpose_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    transpose_panel->SetSizer(transpose_panel_sizer);

    defview = new DefinitionView(transpose_panel, 0);

    /* transpose button panel */
    auto transpose_button_panel = new wxPanel(transpose_panel);
    auto transpose_button_panel_sizer = new wxBoxSizer(wxVERTICAL);
    transpose_button_panel->SetSizer(transpose_button_panel_sizer);

    auto transpose_button_panel_top = new wxPanel(transpose_button_panel);
    auto transpose_button_panel_top_sizer = new wxBoxSizer(wxHORIZONTAL);
    transpose_button_panel_top->SetSizer(transpose_button_panel_top_sizer);

    def_transpose_up = new wxButton(transpose_button_panel_top, ID_DefTransposeUp, _("<"));
    def_transpose_down = new wxButton(transpose_button_panel_top, ID_DefTransposeDown, _(">"));
    def_transpose_to = new wxButton(transpose_button_panel, ID_DefTransposeTo, _("Transpose to..."));

    transpose_button_panel_top_sizer->Add(def_transpose_up);
    transpose_button_panel_top_sizer->Add(def_transpose_down);

    transpose_button_panel_sizer->Add(transpose_button_panel_top);
    transpose_button_panel_sizer->Add(def_transpose_to, 1, wxEXPAND);

    /* finally add the previous components to the transpose panel */
    transpose_panel_sizer->Add(defview, 8, wxEXPAND);
    transpose_panel_sizer->Add(transpose_button_panel, 1, wxEXPAND);

    /* do the splits */
    vertical_splitter->SplitHorizontally(midi, transpose_panel, -60);
    vertical_splitter->SetSashGravity(1);
    left_panel_sizer->Add(vertical_splitter, 1, wxEXPAND);

    /* Right panel */
    auto right_panel = new wxPanel(horizontal_splitter);
    auto right_panel_sizer = new wxBoxSizer(wxVERTICAL);
    right_panel->SetSizer(right_panel_sizer);

    smf_output = new wxButton(right_panel, ID_SmfOutput, _("Write MIDI File"));
    wos_clipboard = new wxButton(right_panel, ID_WosClipboard, _("Copy woslicer II Cutting Positions"));
    def_clipboard = new wxButton(right_panel, ID_DefClipboard, _("Open BMS WAV definitions..."));
    bms_clipboard = new wxButton(right_panel, ID_BmsClipboard, _("Copy BMS Data to Clipboard"));
    open_splitter = new wxButton(right_panel, ID_OpenSplitter, _("Open built-in audio cutter"));
    right_panel_sizer->Add(smf_output, 2, wxEXPAND);
    right_panel_sizer->Add(wos_clipboard, 1, wxEXPAND);
    right_panel_sizer->Add(open_splitter, 1, wxEXPAND);
    right_panel_sizer->Add(def_clipboard, 2, wxEXPAND);
    right_panel_sizer->Add(bms_clipboard, 2, wxEXPAND);

    horizontal_splitter->SplitVertically(left_panel, right_panel, -200);
    horizontal_splitter->SetSashGravity(1);

    midi->SetMidiData(nullptr);
    midi->Show();
}


void DivisionEditor::OnSize(wxSizeEvent &WXUNUSED(event)) {
    // _Size();
    horizontal_splitter->SetSize(GetClientSize());
}

void DivisionEditor::OnSetStatusText(wxCommandEvent &event) {
    frame->UpdateStatus(event.GetString());
}

void DivisionEditor::SetDivision(Division *_div) {
    division = _div;
    if (division) {
        midi->SetMidiData(division);
        defview->SetDivision(division);
        smf_output->Enable(true);
        wos_clipboard->Enable(true);
        def_clipboard->Enable(true);
        bms_clipboard->Enable(true);
        open_splitter->Enable(true);
        def_transpose_up->Enable(true);
        def_transpose_down->Enable(true);
        def_transpose_to->Enable(true);
    } else {
        midi->SetMidiData(nullptr);
        defview->SetDivision(nullptr);
        smf_output->Enable(false);
        wos_clipboard->Enable(false);
        def_clipboard->Enable(false);
        bms_clipboard->Enable(false);
        open_splitter->Enable(false);
        def_transpose_up->Enable(false);
        def_transpose_down->Enable(false);
        def_transpose_to->Enable(false);
    }
}


void DivisionEditor::_SmfOut() {
    if (!division) return;
    auto filename = wxFileSelector(
            _("MIDI files"),
            wxString(),
            wxString(),
            _("mid"),
            midi_file_filter,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
            this
    );

    if (filename.empty()) return;

    if (!division->save_smf(filename)) {
        wxMessageBox(_("Failed to export MIDI file."), app_name, wxOK | wxICON_EXCLAMATION, this);
    }
}

void DivisionEditor::_DivCopy() {
    if (!division) return;
    wxString data = division->get_div_sequence_data();
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(data));
        wxTheClipboard->Close();
    } else {
        wxMessageBox(_("Clipboard is not available."), app_name, wxOK | wxICON_EXCLAMATION, this);
    }
}

void DivisionEditor::_SeqCopy() {
    if (!division) return;
    wxString data = division->get_bms_sequence_data(frame->project->GetSource());
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(data));
        wxTheClipboard->Close();
    } else {
        wxMessageBox(_("Clipboard is not available."), app_name, wxOK | wxICON_EXCLAMATION, this);
    }
}

void DivisionEditor::OnDefTransposeUp(wxCommandEvent &WXUNUSED(event)) {
    division->def_transpose_up();
    defview->ReloadDivision();
    defview->Update();
}

void DivisionEditor::OnDefTransposeDown(wxCommandEvent &WXUNUSED(event)) {
    division->def_transpose_down();
    defview->ReloadDivision();
    defview->Update();
}


class DefTransposeDialog : public wxDialog {

    enum {
        ID_Edit = 1
    };

    wxTextCtrl *edit;
    wxButton *ok;
    ZZNumber data;
    bool zz_enabled;

public:
    DefTransposeDialog(wxWindow *owner, ZZNumber nbegin, bool zz_enabled) :
            wxDialog(owner, -1, _("WAV definitions"), wxDefaultPosition, wxSize(200, 120), wxDEFAULT_DIALOG_STYLE),
            edit(new wxTextCtrl(this, ID_Edit)),
            ok(new wxButton(this, wxID_OK, _("OK"))),
            data(nbegin), zz_enabled(zz_enabled) {
        int w, h;
        this->GetClientSize(&w, &h);
        edit->SetSize(5, 5, w - 10, h - 40);
        ok->SetSize(5, h - 30, w - 10, 25);
        SetEscapeId(wxID_CANCEL);
        edit->SetValue(nbegin.to_string());
    }

    ~DefTransposeDialog() {}

    ZZNumber get_value() {
        return data;
    }

    void OnOK(wxCommandEvent &WXUNUSED(ev)) {
        wxString s = edit->GetValue();
        if (s.length() != 2 || !data.from_string(s) || (!zz_enabled && !data.in_ff())) {
            MessageBeep(0); // windows
            return;
        }
        this->EndDialog(wxID_OK);
    }

DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(DefTransposeDialog, wxDialog)
    EVT_COMMAND(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, DefTransposeDialog::OnOK)
END_EVENT_TABLE()


void DivisionEditor::OnDefTransposeTo(wxCommandEvent &WXUNUSED(event)) {
    if (division->definitions_count() == 0) return;
    auto dialog = new DefTransposeDialog(frame, division->definition(0).zz, division->is_zz_enabled());
    int ret = dialog->ShowModal();
    if (ret != wxID_OK) {
        return;
    }
    ZZNumber zz = dialog->get_value();
    division->def_transpose_to(zz);
    defview->ReloadDivision();
    defview->Update();
}



//--------------------------------------------------------------------------------------



class DefDialog : public wxDialog {

    enum {
        ID_Label = 1,
        ID_Name,
        ID_Data,
        ID_COPY
    };

    Division *div;
    wxStaticText *label;
    wxTextCtrl *name;
    wxTextCtrl *data;
    wxButton *copy;
    wxButton *close;

    void _Size() {
        wxSize size = GetClientSize();
        if (label) label->SetSize(5, 8, 80, 15);
        if (name) name->SetSize(85, 5, size.x - 90, 18);
        if (data) data->SetSize(5, 30, size.x - 10, size.y - 65);
        if (copy) copy->SetSize(5, size.y - 30, (size.x - 10) / 2, 25);
        if (close) close->SetSize((size.x - 10) / 2 + 5, size.y - 30, (size.x - 10) / 2, 25);
    }

    void _NameUpdated() {
        wxString n = name->GetValue();
        wxString d;
        int ofs = div->head_margin_exists() ? 1 : 0;
        for (size_t i = 0; i < div->definitions_count(); i++) {
            const Definition &def = div->definition(i);
            d += wxString::Format(_("#WAV%s %s_%03d.wav\r\n"), def.zz.to_string(), n, def.div + ofs);
        }
        data->SetValue(d);
    }

    void OnSize(wxSizeEvent &e) { _Size(); }

    void OnNameUpdate(wxCommandEvent &e) { _NameUpdated(); }

public:
    DefDialog(wxWindow *owner, Division *_div, const wxString &def_name = _("untitled")) :
            wxDialog(owner, -1, _("BMS WAV definitions"), wxDefaultPosition, wxSize(400, 300),
                     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
            label(new wxStaticText(this, ID_Label, _("Filename (&F):"))),
            name(new wxTextCtrl(this, ID_Name)),
            data(new wxTextCtrl(this, ID_Data, _(""), wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxTE_PROCESS_TAB)),
            copy(new wxButton(this, ID_COPY, _("Copy to clipboard (&C)"))),
            close(new wxButton(this, wxID_CANCEL, _("Close (&W)"))),
            div(_div) {
        SetEscapeId(wxID_CANCEL);
        _Size();
        name->SetValue(def_name.empty() ? _("untitled") : def_name);
        copy->SetDefault();
    }

    ~DefDialog() {}

    void OnCopy(wxCommandEvent &WXUNUSED(event)) {
        wxString s = data->GetValue();
        if (wxTheClipboard->Open()) {
            wxTheClipboard->SetData(new wxTextDataObject(s));
            wxTheClipboard->Close();
        } else {
            wxMessageBox(_("Clipboard is not available."), app_name, wxOK | wxICON_EXCLAMATION, this);
        }
    }

DECLARE_EVENT_TABLE()
};

wxBEGIN_EVENT_TABLE(DefDialog, wxDialog)
    EVT_COMMAND(DefDialog::ID_COPY, wxEVT_COMMAND_BUTTON_CLICKED, DefDialog::OnCopy)
    EVT_SIZE(DefDialog::OnSize)
    EVT_TEXT(DefDialog::ID_Name, OnNameUpdate)
wxEND_EVENT_TABLE()

void DivisionEditor::DefOut() {
    if (!division) return;
    wxString def_name = division->get_name();
    DefDialog dialog(frame, division, def_name);
    dialog.ShowModal();
    dialog.Destroy();
}

bool DivisionEditor::DivRename() {
    auto name = division->get_name();
    wxTextEntryDialog d(
            this,
            wxString::Format(_("Rename '%s' to"), name),
            _("Rename division"),
            name
    );

    if (d.ShowModal() == wxID_OK) {
        division->set_name(d.GetValue());
        if (division->get_name() != name)
            return true;
    }

    return false;
}

void DivisionEditor::OnOpenAudioSplitter(wxCommandEvent &event) {
    if (!division) return;

    auto splitter = new AudioSplitter(this, wxDefaultPosition, wxSize(800, -1), division);
    splitter->Show();
}


