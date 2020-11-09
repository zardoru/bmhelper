

#include "midi_data.h"


static const NodeName _qntz_ = StringToNodeName("qntz");
static const NodeName _note_ = StringToNodeName("note");
static const NodeName _ptch_ = StringToNodeName("ptch");
static const NodeName _cchg_ = StringToNodeName("ccgh");

struct _MidiCCEvent : public MidiParamEvent{
	int cc_num;
};

bool MidiData::read_tree(TreeNode &node){
	init();
	for (TreeNode::iterator sub=node.begin(); sub!=node.end(); sub++){
		if (sub->get_name() == _qntz_){
			if (sub->get_data_size() == sizeof(int)) sub->get_data(&quantize, sizeof(int));
		}else if (sub->get_name() == _note_){
			MidiNoteEvent e;
			if (sub->get_data_size() == sizeof(MidiNoteEvent)){
				sub->get_data(&e, sizeof(MidiNoteEvent));
				note_events.push_back(e);
			}
		}else if (sub->get_name() == _ptch_){
			MidiParamEvent e;
			if (sub->get_data_size() == sizeof(MidiParamEvent)){
				sub->get_data(&e, sizeof(MidiParamEvent));
				pb_events.push_back(e);
			}
		}else if (sub->get_name() == _cchg_){
			_MidiCCEvent e;
			if (sub->get_data_size() == sizeof(_MidiCCEvent)){
				sub->get_data(&e, sizeof(_MidiCCEvent));
				MidiCCLanesMap::iterator i = cc_lanes.find(e.cc_num);
				if (i == cc_lanes.end()){
					std::pair< MidiCCLanesMap::iterator, bool > x;
					x = cc_lanes.insert(MidiCCLanePair(e.cc_num, MidiParamsLane()));
					i = x.first;
				}
				i->second.push_back(*static_cast<MidiParamEvent*>(&e));
			}
		}
	}
	return true;
}

bool MidiData::write_tree(TreeNode &node){
	node.push_back(_qntz_);
	node.back().set_data(&quantize, sizeof(int));
	for (MidiNotesLane::iterator i=note_events.begin(); i!=note_events.end(); i++){
		node.push_back(_note_);
		node.back().set_data(&(*i), sizeof(MidiNoteEvent));
	}
	for (MidiParamsLane::iterator i=pb_events.begin(); i!=pb_events.end(); i++){
		node.push_back(_ptch_);
		node.back().set_data(&(*i), sizeof(MidiParamEvent));
	}
	for (MidiCCLanesMap::iterator lane=cc_lanes.begin(); lane!=cc_lanes.end(); lane++){
		for (MidiParamsLane::iterator i=lane->second.begin(); i!=lane->second.end(); i++){
			_MidiCCEvent e;
			e.position = i->position;
			e.value = i->value;
			e.cc_num = lane->first;
			node.push_back(_cchg_);
			node.back().set_data(&e, sizeof(_MidiCCEvent));
		}
	}
	return true;
}




