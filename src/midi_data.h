
#pragma once

#include "common.h"
#include "midi_base.h"
#include "midi_events.h"
#include "serialize.h"
#include <vector>
#include <map>
#include <set>



class MidiData{

	friend class MidiData_SmfReader;

public:
	typedef std::vector<MidiNoteEvent> MidiNotesLane;
	typedef std::vector<MidiParamEvent> MidiParamsLane;
	typedef std::map<int, MidiParamsLane> MidiCCLanesMap;
	typedef std::pair<int, MidiParamsLane> MidiCCLanePair;

private:
	int quantize;

	MidiNotesLane note_events;	// ノート
	MidiParamsLane pb_events;	// ピッチベンド
	MidiCCLanesMap cc_lanes;	// コントロールチェンジ


public:

	MidiData(int _quantize=480) : quantize(_quantize){}
	~MidiData(){}
	void init(){
		quantize = 480;
		note_events.clear();
		pb_events.clear();
		cc_lanes.clear();
	}

	bool load_smf(const wxString &filename);
	bool save_smf(const wxString &filename) const;

	virtual bool read_tree(TreeNode &node);
	virtual bool write_tree(TreeNode &node);

	int get_quantize() const{ return quantize; }
	MidiNotesLane::iterator notes_begin(){ return note_events.begin(); }
	MidiNotesLane::iterator notes_end(){ return note_events.end(); }
	MidiNotesLane::reverse_iterator notes_rbegin(){ return note_events.rbegin(); }
	MidiNotesLane::reverse_iterator notes_rend(){ return note_events.rend(); }
	size_t notes_count() const{ return note_events.size(); }
	MidiNoteEvent &notes(size_t index){ return note_events[index]; }
	const MidiNoteEvent &notes(size_t index) const{ return note_events[index]; }
	void note_push_back(const MidiNoteEvent &ev){ note_events.push_back(ev); }

};


