#ifndef BMHELPER_AUDIOSPLITTER_H
#define BMHELPER_AUDIOSPLITTER_H

#include "common.h"
#include "division.h"
#include <wx/spinctrl.h>

class AudioSplitter : public wxFrame {
    Division* division;
    wxTextCtrl *filename_ctrl;
    wxTextCtrl *output_dir_ctrl;
    wxTextCtrl *output_prefix_ctrl;

    wxSpinCtrlDouble *bpm_ctrl;
    wxSpinCtrlDouble *offset_ctrl;
    wxPanel *panel;
public:
    AudioSplitter(wxWindow *parent, const wxPoint &pos, const wxSize& size, Division* div);

    void OnClose(wxCommandEvent &evt);
    void OnSplit(wxCommandEvent &evt);
    void OnBrowseInput(wxCommandEvent &evt);
    void OnBrowseOutput(wxCommandEvent &evt);
    void OnDropFiles(wxDropFilesEvent &evt);
    void OnSize(wxSizeEvent &evt);
    wxDECLARE_EVENT_TABLE();

    void SetInputAudioFile(wxString input_file);

    wxFileName &GetDefaultDialogPath(wxFileName &path) const;
};


#endif //BMHELPER_AUDIOSPLITTER_H
