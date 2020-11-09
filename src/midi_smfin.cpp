
#include "midi_data.h"
#include "smf_io.h"
#include <wx/file.h>
#include <wx/dialog.h>
#include <algorithm>


struct MidiData_PreLoader : public SmfRandomReceiver {
    std::string track_name;
    bool channels[16]{};

    MidiData_PreLoader() {
        for (int i = 0; i < 16; i++) {
            channels[i] = false;
        }
    }

    void ReceiveNote(int position, int channel, int gate, int nn, int v) override {
        channels[channel] = true;
    }

    void ReceiveMeta(int position, int type, const std::vector<unsigned char> &data) override {
        if (type == 0x03) {
            track_name.insert(track_name.begin(), data.begin(), data.end());
        }
    }

    bool using_any_channel() {
        bool b = false;
        for (bool channel : channels) {
            b |= channel;
        }
        return b;
    }
};

class LoadSmfDialog : public wxDialog {
    std::vector<MidiData_PreLoader> &tracks;
    wxButton *button_ok;
    wxComboBox *combo_trk;
    wxCheckBox *check_chs[16];
    enum {
        ID_Tracks_Text = 0x101,
        ID_Tracks_Choice = 0x102,
        ID_Channels_Text = 0x111,
        ID_Channels_Check = 0x120
    };

    void _on_size() {
        wxSize size = GetClientSize();
        if (combo_trk) combo_trk->SetSize(95, 5, size.x - 100, 15);
        if (button_ok) button_ok->SetSize(size.x - 125, size.y - 30, 120, 25);
    }

public:
    LoadSmfDialog(std::vector<MidiData_PreLoader> &_tracks)
            : wxDialog(0, -1, _("MIDI file import settings"), wxDefaultPosition,
                       wxSize(290, 185), wxDEFAULT_DIALOG_STYLE & ~wxCLOSE_BOX | wxRESIZE_BORDER),
              tracks(_tracks), combo_trk(0), button_ok(0) {
        size_t def = 0;
        for (size_t i = 0; i < tracks.size(); i++) {
            if (tracks[i].using_any_channel()) {
                def = i;
                break;
            }
        }
        auto track_choices = std::vector<wxString>(tracks.size());
        for (size_t i = 0; i < tracks.size(); i++) {
            track_choices[i] = wxString::Format(_("%04X: %s"), (int) i, tracks[i].track_name.c_str());
        }

        new wxStaticText(this, ID_Tracks_Text, _("Tracks (&T):"), wxPoint(5, 8), wxSize(85, 15));
        combo_trk = new wxComboBox(this, ID_Tracks_Choice, track_choices[def], wxPoint(95, 5),
                                   wxSize(200, 15), tracks.size(), track_choices.data(), wxCB_DROPDOWN | wxCB_READONLY);
        combo_trk->SetSelection(def);
        new wxStaticText(this, ID_Tracks_Text, _("Channels (&C):"), wxPoint(5, 38), wxSize(85, 15));
        for (int i = 0; i < 16; i++) {
            int ix = i % 4, iy = i / 4;
            check_chs[i] = new wxCheckBox(this, ID_Channels_Check + i, wxString::Format(_("%02X"), i),
                                          wxPoint(95 + ix * 40, 35 + iy * 20), wxSize(35, 15));
            check_chs[i]->SetValue(true);
        }
        button_ok = new wxButton(this, wxID_OK, _("OK"));
        button_ok->SetDefault();
        SetMinSize(wxSize(290, 185));
        SetEscapeId(wxID_NONE);
        _on_size();
    }

    ~LoadSmfDialog() override = default;

    void OnSize(wxSizeEvent &WXUNUSED(ev)) { _on_size(); }

DECLARE_EVENT_TABLE()
public:
    size_t get_track() {
        return (size_t) combo_trk->GetCurrentSelection();
    }

    bool get_channel_mask(int ch) {
        return check_chs[ch]->GetValue();
    }
};

wxBEGIN_EVENT_TABLE(LoadSmfDialog, wxDialog)
    EVT_SIZE(LoadSmfDialog::OnSize)
wxEND_EVENT_TABLE()


class MidiData_SmfReader : public SmfRandomReceiver {
    MidiData &midi;
    bool (&ch)[16];
public:
    MidiData_SmfReader(MidiData &_midi, bool (&channel_mask)[16]) : midi(_midi), ch(channel_mask) {}

    virtual void ReceiveNote(int position, int channel, int gate, int nn, int v) {
        if (!ch[channel]) return;
        midi.note_events.push_back(MidiNoteEvent(position, gate, nn, v));
    }
/*
	virtual void ReceiveControlChange(int position, int channel, int ccn, int v){
		if (!ch[channel]) return;
		MidiData::MidiCCLanesMap::iterator i = midi.cc_lanes.find(ccn);
		if (i == midi.cc_lanes.end()){
			std::pair< MidiData::MidiCCLanesMap::iterator, bool > x;
			x = midi.cc_lanes.insert(MidiData::MidiCCLanePair(ccn, MidiData::MidiParamsLane()));
			i = x.first;
		}
		i->second.push_back(MidiParamEvent(position, v));
	}
	virtual void ReceivePitchBend(int position, int channel, int v){
		if (!ch[channel]) return;
		midi.pb_events.push_back(MidiParamEvent(position, v));
	}
*/
};


bool MidiData::load_smf(const wxString &filename) {
    SmfData smf;
    if (!smf.load(filename)) {
        return false;
    }
    quantize = smf.get_delta_time();
    size_t track_count = smf.get_track_count();
    if (!track_count) return false;

    init(); /* don't write on top of existing MIDI data in case of reload */

    std::vector<MidiData_PreLoader> track_pre(track_count);
    for (size_t i = 0; i < track_count; i++) {
        SmfDataAccessor sda(smf, i);
        sda.GetAllEvents(track_pre[i]);
    }
    auto dialog = new LoadSmfDialog(track_pre);
    dialog->ShowModal();
    bool channel_mask[16];

    for (int i = 0; i < 16; i++)
        channel_mask[i] = dialog->get_channel_mask(i);

    MidiData_SmfReader reader(*this, channel_mask);
    SmfDataAccessor acc(smf, dialog->get_track());
    acc.GetAllEvents(reader);
    sort(note_events.begin(), note_events.end());
    dialog->Destroy();
    return true;
}



