
#include "smf_io.h"


SmfDataAccessor::SmfDataAccessor(SmfData &smf, size_t track_num)
: track(smf.get_or_create_track(track_num))
{
}


void SmfDataAccessor::Initialize(int position_end){
	track.clear();
	SmfEvent ev;
	ev.type = SMF_EVENT_META;
	ev.meta.type = 0x2F;
	ev.var_data.clear();
	track.insert(SmfData::EventsTimelineElement(position_end, ev));
}

void SmfDataAccessor::EndOfTrack(int position){
	if (position < 0){
		position = track.empty()? 0 : position = track.rbegin()->first;
	}
	SmfEvent ev;
	ev.type = SMF_EVENT_META;
	ev.meta.type = 0x2F;
	ev.var_data.clear();
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::Note(int position, int channel, int gate, int nn, int v){
	NoteOn(position, channel, nn, v);
	NoteOff(position+gate, channel, nn, 0);
}

void SmfDataAccessor::NoteOn(int position, int channel, int nn, int v){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_NOTE_ON;
	ev.note_on.nn = nn;
	ev.note_on.v  = v;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::NoteOff(int position, int channel, int nn, int v){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_NOTE_OFF;
	ev.note_off.nn = nn;
	ev.note_off.v  = v;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::PolyKeyPressure(int position, int channel, int nn, int v){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_PLKY_PRS;
	ev.plky_prs.nn = nn;
	ev.plky_prs.v  = v;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::ControlChange(int position, int channel, int ccn, int v){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_CTRL_CHG;
	ev.ctrl_chg.ccn = ccn;
	ev.ctrl_chg.v  = v;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::ProgramChange(int position, int channel, int pn){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_PROG_CHG;
	ev.prog_chg.pn = pn;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::ChannelPressure(int position, int channel, int v){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_CHNL_PRS;
	ev.chnl_prs.v = v;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::PitchBend(int position, int channel, int v){
	SmfEvent ev;
	ev.channel = channel;
	ev.type = SMF_EVENT_PTCH_BND;
	ev.ptch_bnd.v = v;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::SystemExclusiveF0(int position, const std::vector<unsigned char> &data){
	SmfEvent ev;
	ev.type = SMF_EVENT_SYSX_F0;
	ev.var_data = data;
	ev.var_data.push_back(0xF7);
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::SystemExclusiveF7(int position, const std::vector<unsigned char> &data){
	SmfEvent ev;
	ev.type = SMF_EVENT_SYSX_F0;
	ev.var_data = data;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}

void SmfDataAccessor::Meta(int position, int type, const std::vector<unsigned char> &data){
	SmfEvent ev;
	ev.type = SMF_EVENT_META;
	ev.meta.type = type;
	ev.var_data = data;
	track.insert(SmfData::EventsTimelineElement(position, ev));
}


void SmfDataAccessor::GetAllEvents(SmfRandomReceiver &receiver){
	struct NoteOnInfo{
		int position;
		int v;
	} noi{};
	std::map<int, NoteOnInfo> keyboard;
	std::map<int, NoteOnInfo>::iterator key;
	std::vector<unsigned char> var_data;
	for (const auto & i : track){
		int position = i.first;
		const SmfEvent &ev = i.second;
		switch (ev.type){
			case SMF_EVENT_NOTE_OFF:
				receiver.ReceiveNoteOff(position, ev.channel, ev.note_off.nn, ev.note_off.v);
				if ((key = keyboard.find(ev.note_off.nn)) != keyboard.end()){
					receiver.ReceiveNote(
					        key->second.position,
					        ev.channel,
					        position-key->second.position,
					        key->first,
					        key->second.v
                    );

					keyboard.erase(key);
				}
				break;
			case SMF_EVENT_NOTE_ON:
				receiver.ReceiveNoteOn(position, ev.channel, ev.note_on.nn, ev.note_on.v);
				noi.position = position;
				noi.v = ev.note_on.v;
				keyboard.insert(std::pair<int, NoteOnInfo>(ev.note_on.nn, noi));
				break;
			case SMF_EVENT_PLKY_PRS:
				receiver.ReceivePolyKeyPressure(position, ev.channel, ev.plky_prs.nn, ev.plky_prs.v);
				break;
			case SMF_EVENT_CTRL_CHG:
				receiver.ReceiveControlChange(position, ev.channel, ev.ctrl_chg.ccn, ev.ctrl_chg.v);
				break;
			case SMF_EVENT_PROG_CHG:
				receiver.ReceiveProgramChange(position, ev.channel, ev.prog_chg.pn);
				break;
			case SMF_EVENT_CHNL_PRS:
				receiver.ReceiveChannelPressure(position, ev.channel, ev.chnl_prs.v);
				break;
			case SMF_EVENT_PTCH_BND:
				receiver.ReceivePitchBend(position, ev.channel, ev.ptch_bnd.v);
				break;
			case SMF_EVENT_SYSX_F0:
				var_data = ev.var_data;
				var_data.pop_back();	// ����F7
				receiver.ReceiveSystemExclusiveF0(position, var_data);
				break;
			case SMF_EVENT_SYSX_F7:
				receiver.ReceiveSystemExclusiveF7(position, ev.var_data);
				break;
			case SMF_EVENT_META:
				receiver.ReceiveMeta(position, ev.meta.type, ev.var_data);
				break;
		}
	}
}


