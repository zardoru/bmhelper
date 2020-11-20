#include <wx/filename.h>
#include "AudioSplitter.h"
#include "wavsplit.h"

enum {
    ID_Split,
    ID_FilenameCtrl,
    ID_OutPathCtrl,
    ID_BrowseFilename,
    ID_BrowseOutPath,
    ID_OutputPrefix
};


wxBEGIN_EVENT_TABLE(AudioSplitter, wxFrame)
    EVT_COMMAND(ID_Split, wxEVT_COMMAND_BUTTON_CLICKED, AudioSplitter::OnSplit)
    EVT_COMMAND(ID_BrowseFilename, wxEVT_COMMAND_BUTTON_CLICKED, AudioSplitter::OnBrowseInput)
    EVT_COMMAND(ID_BrowseOutPath, wxEVT_COMMAND_BUTTON_CLICKED, AudioSplitter::OnBrowseOutput)
    EVT_COMMAND(wxID_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, AudioSplitter::OnClose)
    EVT_DROP_FILES(AudioSplitter::OnDropFiles)
wxEND_EVENT_TABLE()

AudioSplitter::AudioSplitter(wxWindow *parent, const wxPoint &pos, const wxSize &size, Division *div)
    : wxFrame(parent, wxID_ANY,
              wxString::Format(_("Audio splitter (division: \"%s\")"), div->get_name()),
              pos, size),
      division(div) {
    auto sizer = new wxBoxSizer(wxVERTICAL);

    /* audio file input */
    auto panel_input = new wxPanel(this);
    auto panel_input_sizer = new wxStaticBoxSizer(wxVERTICAL, panel_input, _("Audio File"));
    panel_input->SetSizer(panel_input_sizer);

    auto lbl = new wxStaticText(panel_input, wxID_ANY, _("You can drag and drop a .wav file into this window."));
    panel_input_sizer->Add(lbl, 0, wxEXPAND);

    auto panel_input_second_row = new wxBoxSizer(wxHORIZONTAL);
    filename_ctrl = new wxTextCtrl(panel_input, ID_FilenameCtrl);
    filename_ctrl->SetToolTip(new wxToolTip(_("You can drag and drop a wav file into this window.")));
    panel_input_second_row->Add(filename_ctrl, 1);

    panel_input_second_row->AddSpacer(5);

    auto browse_btn = new wxButton(panel_input, ID_BrowseFilename, _("Browse..."));
    panel_input_second_row->Add(browse_btn, 0);
    panel_input_sizer->Add(panel_input_second_row, 1, wxEXPAND);

    /* audio file output */
    auto panel_output = new wxPanel(this);
    auto panel_output_sizer = new wxStaticBoxSizer(wxVERTICAL, panel_output, _("Output Directory"));
    panel_output->SetSizer(panel_output_sizer);

    /* 1st row */
    auto panel_output_path_sizer = new wxBoxSizer(wxHORIZONTAL);

    output_dir_ctrl = new wxTextCtrl(panel_output, ID_OutPathCtrl);
    panel_output_path_sizer->Add(output_dir_ctrl, 2);

    panel_output_path_sizer->AddSpacer(5);

    auto browse_output_btn = new wxButton(panel_output, ID_BrowseOutPath, _("Browse..."));
    panel_output_path_sizer->Add(browse_output_btn, 0);

    panel_output_sizer->Add(panel_output_path_sizer, 2, wxEXPAND);

    panel_output_sizer->AddSpacer(20);

    /* 2nd row */
    auto panel_output_filename_sizer = new wxBoxSizer(wxHORIZONTAL);
    panel_output_filename_sizer->Add(new wxStaticText(panel_output, wxID_ANY, _("Output sound file prefix:")), 1);

    panel_output_filename_sizer->AddSpacer(20);

    output_prefix_ctrl = new wxTextCtrl(panel_output, ID_OutputPrefix, div->get_name());
    panel_output_filename_sizer->Add(output_prefix_ctrl, 2);
    output_prefix_ctrl->SetToolTip(new wxToolTip(_("Should be the same as the BMS WAV definitions.")));

    panel_output_sizer->Add(panel_output_filename_sizer, 2, wxEXPAND);

    /* slice parameters */
    auto panel_parameters = new wxPanel(this);
    auto panel_parameters_sizer = new wxStaticBoxSizer(wxHORIZONTAL, panel_parameters, _("Slice Parameters"));
    panel_parameters->SetSizer(panel_parameters_sizer);

    auto lbl_bpm = new wxStaticText(panel_parameters, wxID_ANY, "BPM:");
    panel_parameters_sizer->Add(lbl_bpm, 1);

    bpm_ctrl = new wxSpinCtrlDouble(panel_parameters);
    bpm_ctrl->SetRange(1, 5000);
    bpm_ctrl->SetDigits(3);
    bpm_ctrl->SetValue(120);
    panel_parameters_sizer->Add(bpm_ctrl, 3);

    panel_parameters_sizer->AddSpacer(20);

    auto lbl_offset = new wxStaticText(panel_parameters, wxID_ANY, "Offset (MS):");
    panel_parameters_sizer->Add(lbl_offset, 1);

    offset_ctrl = new wxSpinCtrlDouble(panel_parameters);
    offset_ctrl->SetRange(-10000, 10000);
    offset_ctrl->SetValue(0);
    panel_parameters_sizer->Add(offset_ctrl, 2);


    /* Actions */
    auto panel_bottom = new wxPanel(this);
    auto panel_bottom_sizer = new wxBoxSizer(wxHORIZONTAL);
    panel_bottom->SetSizer(panel_bottom_sizer);


    auto split_btn = new wxButton(panel_bottom, ID_Split, _("Split"));
    auto close_btn = new wxButton(panel_bottom, wxID_CLOSE, _("Close"));
    panel_bottom_sizer->Add(split_btn, 3);
    panel_bottom_sizer->AddSpacer(20);
    panel_bottom_sizer->Add(close_btn, 3);


    sizer->Add(panel_input, 1, wxEXPAND);
    sizer->Add(panel_output, 1, wxEXPAND);
    sizer->Add(panel_parameters, 1, wxEXPAND);
    sizer->Add(panel_bottom, 0, wxEXPAND);

    SetSizerAndFit(sizer);
    SetMinSize(size);

    DragAcceptFiles(true);
}


void AudioSplitter::OnSplit(wxCommandEvent &evt) {
    try {
        SplitFile(
            filename_ctrl->GetValue(),
            output_dir_ctrl->GetValue(),
            output_prefix_ctrl->GetValue(),
            division,
            bpm_ctrl->GetValue(),
            offset_ctrl->GetValue()
        );
    } catch(std::exception &e) {
        wxMessageBox(e.what(), _("Error while splitting file"), wxOK | wxCENTRE | wxICON_ERROR);
    }
}

void AudioSplitter::OnBrowseInput(wxCommandEvent &evt) {
    wxFileName path = wxFileName::GetHomeDir();
    path = GetDefaultDialogPath(path);

    auto dlg = new wxFileDialog(
            this,
            "Select audio file...",
            path.GetFullPath(),
            "",
            "WAV files (*.wav)",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (dlg->ShowModal() == wxID_CANCEL)
        return;

    SetInputAudioFile(dlg->GetPath());
    dlg->Destroy();
}

wxFileName &AudioSplitter::GetDefaultDialogPath(wxFileName &path) const {
    if (!filename_ctrl->GetValue().empty()) {
        path = filename_ctrl->GetValue();
        path = path.GetPath();
    } else {
        if (!output_dir_ctrl->GetValue().empty()) {
            path = output_dir_ctrl->GetValue();
            path = path.GetPath();
        }
    }
    return path;
}

void AudioSplitter::OnBrowseOutput(wxCommandEvent &evt) {
    wxFileName path = wxFileName::GetHomeDir();
    path = GetDefaultDialogPath(path);

    auto dlg = new wxDirDialog(
            this,
            _("Select audio output directory..."),
            path.GetPath()
    );

    if (dlg->ShowModal() == wxID_CANCEL)
        return;

    wxFileName path_nosep (dlg->GetPath());
    output_dir_ctrl->SetValue(path_nosep.GetPathWithSep());
    dlg->Destroy();
}

void AudioSplitter::OnDropFiles(wxDropFilesEvent &evt) {
    if (evt.GetNumberOfFiles() == 0)
        return;

    SetInputAudioFile(evt.GetFiles()[0]);
}

void AudioSplitter::SetInputAudioFile(wxString input_file) {
    if (filename_ctrl->GetValue() != input_file) {
        filename_ctrl->SetValue(input_file);
    }

    if (output_dir_ctrl->GetValue().empty()) {
        wxFileName fn(input_file);
        output_dir_ctrl->SetValue(fn.GetPathWithSep());
    }
}

void AudioSplitter::OnClose(wxCommandEvent &evt) {
    Close(true);
}


