
#include <string>
#include "smf_io.h"


SmfData::SmfData(int _delta_time) : delta_time(_delta_time)
{
}

SmfData::~SmfData(){
	clear();
}

void SmfData::clear(){
	for (TracksVector::iterator i=tracks.begin(); i!=tracks.end(); i++){
		delete *i;
	}
	tracks.clear();
}


SmfData::EventsTimeline &SmfData::get_or_create_track(size_t track_num){
	size_t old_size = tracks.size();
	if (track_num >= old_size){
		tracks.resize(track_num+1);
		for (size_t i=old_size; i<=track_num; i++){
			tracks[i] = new EventsTimeline;
		}
	}
	return *tracks[track_num];
}


inline unsigned short read_word_be(const unsigned char *src){
	return (src[0]<<8) | (src[1]);
}
inline unsigned long read_dword_be(const unsigned char *src){
	return (src[0]<<24) | (src[1]<<16) | (src[2]<<8) | src[3];
}

inline void write_word_be(unsigned short v, unsigned char *dst){
	dst[0] = (v & 0xff00) >> 8;
	dst[1] = (v & 0x00ff);
}
inline void write_dword_be(unsigned long v, unsigned char *dst){
	dst[0] = (v & 0xff000000) >> 24;
	dst[1] = (v & 0x00ff0000) >> 16;
	dst[2] = (v & 0x0000ff00) >> 8;
	dst[3] = (v & 0x000000ff);
}

inline void write_word_be(unsigned short v, wxFile &dst){
	int c;
	c = (v & 0xff00) >> 8;	dst.Write(&c, 1);
	c = v & 0x00ff;			dst.Write(&c, 1);
}
inline void write_dword_be(unsigned long v, wxFile &dst){
	int c;
	c = (v & 0xff000000) >> 24;	dst.Write(&c, 1);
	c = (v & 0x00ff0000) >> 16;	dst.Write(&c, 1);
	c = (v & 0x0000ff00) >> 8;	dst.Write(&c, 1);
	c = v & 0x000000ff;			dst.Write(&c, 1);
}



bool SmfData::load(const wxString &filename){
	clear();
	wxFile fin(filename, wxFile::read);
	if (!fin.IsOpened()) return false;
	fin.SeekEnd();
	size_t file_size = fin.Tell();
	unsigned char *buf = new unsigned char[file_size];
	fin.Seek(0);
	fin.Read(buf, file_size);
	fin.Close();
	try{
		// header
		int format;
		int tracks_count;
		// chunk rw
		int ptr  = 0;
		unsigned long size;
		// read header
		std::string h((char*)&buf[ptr], 4); ptr += 4;
		if (h.compare("MThd")) throw 0;
		size = read_dword_be(&buf[ptr]); ptr += 4;
		if (size != 6) throw 1;
		format = read_word_be(&buf[ptr]); ptr += 2;
		if (format != 0 && format != 1) throw 2;
		tracks_count = read_word_be(&buf[ptr]); ptr += 2;
		delta_time = read_word_be(&buf[ptr]); ptr += 2;
		tracks.resize(tracks_count);
		// read tracks
		for (int track_num=0; track_num<tracks_count; track_num++){
			if (ptr >= (int)file_size) throw -1;
			std::string h((char*)&buf[ptr], 4); ptr += 4;
			if (h.compare("MTrk")) throw 3;
			size = read_dword_be(&buf[ptr]); ptr += 4;
			int time = 0;
			int track_end = ptr + size;
			tracks[track_num] = new EventsTimeline;
			int status = 0;
			if (track_end > (int)file_size) throw -2;
			// read events
			while (ptr < track_end){
				int read;
				int dt = SmfReadVarInt(&buf[ptr], read);
				ptr += read;
				time += dt;
				SmfEvent ev;
				int sz = ev.read(&buf[ptr], status);
				if (sz < 0) throw 4;
				ptr += sz;
				tracks[track_num]->insert(EventsTimelineElement(time, ev));
			}
			ptr = track_end;
		}
	}catch(...){
		delete[] buf;
		return false;
	}
	delete[] buf;
	return true;
}





bool SmfData::save(const wxString &filename){
	wxFile fout(filename, wxFile::write);
	try{
		// header
		int format = 1;
		// chunk_rw
		int ptr = 0;
		unsigned long size;
		// write header
		fout.Write("MThd", 4);
		write_dword_be(6, fout);
		write_word_be(format, fout);
		write_word_be(tracks.size(), fout);
		write_word_be(delta_time, fout);
		// write tracks
		for (size_t track_num=0; track_num<tracks.size(); track_num++){
			fout.Write("MTrk", 4);
			write_dword_be(0xcccccccc, fout);	// chunk size(dummy)
			wxFileOffset chunk_start = fout.Tell();
			int time=0;
			int status=0;
			for (EventsTimeline::iterator i=tracks[track_num]->begin(); i!=tracks[track_num]->end(); i++){
				SmfWriteVarInt(i->first - time, fout);
				time = i->first;
				if (!i->second.write(fout, status)){
					throw 1;
				}
			}
			wxFileOffset chunk_end = fout.Tell();
			size = chunk_end - chunk_start;
			fout.Seek(chunk_start-4, wxFromStart);
			write_dword_be(size, fout);
			fout.Seek(chunk_end, wxFromStart);
		}
	}catch(...){
		return false;
	}
	return true;
}






