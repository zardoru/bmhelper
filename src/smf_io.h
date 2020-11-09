
#pragma once

#include "midi_base.h"
#include <wx/file.h>
#include <vector>
#include <map>


enum SmfFormat{
	SMF_FORMAT_0 = 0,
	SMF_FORMAT_1 = 1
};



enum SmfEventType{
	SMF_EVENT_NOTE_OFF,
	SMF_EVENT_NOTE_ON,
	SMF_EVENT_PLKY_PRS,
	SMF_EVENT_CTRL_CHG,
	SMF_EVENT_PROG_CHG,
	SMF_EVENT_CHNL_PRS,
	SMF_EVENT_PTCH_BND,
	SMF_EVENT_SYSX_F0,
	SMF_EVENT_SYSX_F7,
	SMF_EVENT_META
};



struct SmfEvent{
	SmfEventType type;
	int channel;
	std::vector<unsigned char> var_data;	// 可変長データ
	union {
		struct {
			int nn, v;
		} note_on, note_off, plky_prs;
		struct {
			int ccn, v;
		} ctrl_chg;
		struct {
			int pn;
		} prog_chg;
		struct {
			int v;
		} chnl_prs, ptch_bnd;
		struct {
			int type;
		} meta;
	};
	int read(const unsigned char *src, int &status);
	bool read(wxFile &src, int &status);
	int write(unsigned char *dst, int &status) const;
	bool write(wxFile &dst, int &status) const;
};


class SmfDataAccessor;


class SmfData{

	friend class SmfDataAccessor;
	typedef std::multimap<int, SmfEvent> EventsTimeline;
	typedef std::pair<int, SmfEvent> EventsTimelineElement;
	typedef std::vector<EventsTimeline*> TracksVector;

	int delta_time;
	TracksVector tracks;

	EventsTimeline &get_or_create_track(size_t tarck_num);

public:
	SmfData(int _delta_time=480);
	~SmfData();
	void clear();

	bool load(const wxString &filename);
	bool save(const wxString &filename);

	int get_delta_time() const{ return delta_time; }
	size_t get_track_count() const{ return tracks.size(); }

};


class SmfRandomReceiver{
public:
	virtual void ReceiveNote(int position, int channel, int gate, int nn, int v){}
	virtual void ReceiveNoteOn(int position, int channel, int nn, int v){}
	virtual void ReceiveNoteOff(int position, int channel, int nn, int v){}
	virtual void ReceivePolyKeyPressure(int position, int channel, int nn, int v){}
	virtual void ReceiveControlChange(int position, int channel, int ccn, int v){}
	virtual void ReceiveProgramChange(int position, int channel, int pn){}
	virtual void ReceiveChannelPressure(int position, int channel, int v){}
	virtual void ReceivePitchBend(int position, int channel, int v){}
	virtual void ReceiveSystemExclusiveF0(int position, const std::vector<unsigned char> &data){}
	virtual void ReceiveSystemExclusiveF7(int position, const std::vector<unsigned char> &data){}
	virtual void ReceiveMeta(int position, int type, const std::vector<unsigned char> &data){}
};


class SmfDataAccessor{
	
	SmfData::EventsTimeline &track;

public:
	SmfDataAccessor(SmfData &smf, size_t track_num);

	// writer
	void Initialize(int position_end);	// 全イベントを消去してからEndOfTrackイベント挿入
	void EndOfTrack(int position=-1);	// -1のときは最後のイベントの直後に加える
	void Note(int position, int channel, int gate, int nn, int v);
	void NoteOn(int position, int channel, int nn, int v);
	void NoteOff(int position, int channel, int nn, int v);
	void PolyKeyPressure(int position, int channel, int nn, int v);
	void ControlChange(int position, int channel, int ccn, int v);
	void ProgramChange(int position, int channel, int pn);
	void ChannelPressure(int position, int channel, int v);
	void PitchBend(int position, int channel, int v);
	void SystemExclusiveF0(int position, const std::vector<unsigned char> &data);
	void SystemExclusiveF7(int position, const std::vector<unsigned char> &data);
	void Meta(int position, int type, const std::vector<unsigned char> &data);

	// reader
	void GetAllEvents(SmfRandomReceiver &receiver);

};





//----------------------------------------------------------------

inline int SmfReadVarInt(const unsigned char *src, int &read_size){
	read_size = 0;
	int v = 0;
	int c;
	do {
		c = src[read_size++];
		v = (v << 7) | (c & 0x7f);
	} while((c & 0x80) && !(v & 0x0fe00000));
	return v;
}

inline int SmfReadVarInt(wxFile &src){
	int v = 0;
	int c;
	do {
		src.Read(&c, 1);
		v = (v << 7) | (c & 0x7f);
	} while((c & 0x80) && !(v & 0x0fe00000));
	return v;
}

inline void SmfWriteVarInt(int v, unsigned char *dst, int &written_size){
	written_size = 0;
	int a[4];
	int k = 0;
	v &= 0x0fffffff;
	a[k++] = v & 0x7f;
	while (v>>=7, v) a[k++] = (v & 0x7f) | 0x80;
	while (k--){
		dst[written_size++] = a[k];
	}
	return;
}

inline void SmfWriteVarInt(int v, wxFile &dst){
	int a[4];
	int k = 0;
	v &= 0x0fffffff;
	a[k++] = v & 0x7f;
	while (v>>=7, v) a[k++] = (v & 0x7f) | 0x80;
	while (k--){
		dst.Write(&a[k], 1);
	}
	return;
}

