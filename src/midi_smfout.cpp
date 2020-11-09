
#include "midi_data.h"
#include "smf_io.h"
#include <wx/file.h>


bool MidiData::save_smf(const wxString &filename) const{

	SmfData smf(quantize);

	// トラック0
	{
		SmfDataAccessor track(smf, 0);
		track.EndOfTrack(-1);
	}
	// トラック1
	{
		SmfDataAccessor track(smf, 1);
		for (auto note_event : note_events){
			track.Note(note_event.position, 0, note_event.gate, note_event.note_num, note_event.velocity);
		}
		for (auto pb_event : pb_events){
			track.PitchBend(pb_event.position, 0, pb_event.value);
		}
		for (const auto & cc_lane : cc_lanes){
			int ccn = cc_lane.first;
			const MidiParamsLane &lane = cc_lane.second;
			for (auto i : lane){
				track.ControlChange(i.position, 0, ccn, i.value);
			}
		}
		track.EndOfTrack(-1);
	}
	if (!smf.save(filename)) return false;

	return true;
}




