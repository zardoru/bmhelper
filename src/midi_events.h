
#pragma once

#include "common.h"
#include "midi_base.h"


struct MidiEvent{
	int position;

	MidiEvent(){}
	MidiEvent(int pos) : position(pos){}
};

struct MidiLongEvent : public MidiEvent{
	int gate;

	MidiLongEvent(){}
	MidiLongEvent(int pos, int gat) : MidiEvent(pos), gate(gat){}
};

//------------------------------------------------


struct MidiNoteEvent : public MidiLongEvent{
	int note_num;
	int velocity;

	MidiNoteEvent(){}
	MidiNoteEvent(int pos, int gat, int nn, int v) : MidiLongEvent(pos, gat), note_num(nn), velocity(v){}
	bool operator <(const MidiNoteEvent &r) const{
		return this->position < r.position;
	}
	bool operator >(const MidiNoteEvent &r) const{
		return this->position > r.position;
	}
	bool operator <=(const MidiNoteEvent &r) const{
		return this->position <= r.position;
	}
	bool operator >=(const MidiNoteEvent &r) const{
		return this->position >= r.position;
	}
};


struct MidiParamEvent : public MidiEvent{
	int value;

	MidiParamEvent(){}
	MidiParamEvent(int pos, int v) : MidiEvent(pos), value(v){}
	bool operator <(const MidiParamEvent &r) const{
		return this->position < r.position;
	}
	bool operator >(const MidiParamEvent &r) const{
		return this->position > r.position;
	}
	bool operator <=(const MidiParamEvent &r) const{
		return this->position <= r.position;
	}
	bool operator >=(const MidiParamEvent &r) const{
		return this->position >= r.position;
	}
};




