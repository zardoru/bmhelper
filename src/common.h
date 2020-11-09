
#pragma once

#include <wx/wx.h>

extern const char* app_name;
extern const char* app_version;
extern const char* project_file_ext;
extern const char* midi_file_filter;
extern const char* project_file_filter;

enum class SupportedLanguages {
    DEFAULT = wxLANGUAGE_DEFAULT,
    ENGLISH = wxLANGUAGE_ENGLISH, // en_GB
    SPANISH = wxLANGUAGE_SPANISH, // es_ES
    NORWEGIAN = wxLANGUAGE_NORWEGIAN_BOKMAL, // nb_NO
    JAPANESE = wxLANGUAGE_JAPANESE, // ja_JP
    LANG_COUNT = 5 /* update LangNames & Languages @ Configuration.cpp if this changes */
};

extern int Languages[];
extern wxString LangNames[];

class ZZNumber{
	unsigned int low, high;
	static int _x(unsigned int n){ return n>=10 ? 'A'-10+n : '0'+n; }
	static unsigned int _rx(int c){ return c>='A'&&c<='Z' ? c-'A'+10 : (c>='a'&&c<='z' ? c-'a'+10 : c-'0' ); }
public:
	ZZNumber(unsigned int n=0) : low(n%36), high(n/36){ if (high>=36) high%=36; }
	ZZNumber(unsigned int _low, unsigned int _high) : low(_low), high(_high){ if (low>=36) low%=36;  if (high>=36) high%=36; }
	operator int() const{ return high*36 + low; }
	ZZNumber &operator=(unsigned int n){ low = n%36; high = n/36; if (high>=36) high%=36; return *this; }
	bool operator ==(const ZZNumber &zz) const{ return high==zz.high && low==zz.low; }
	bool operator >(const ZZNumber &zz) const{ return high==zz.high? (low>zz.low) : (high>zz.high); }
	bool operator <(const ZZNumber &zz) const{ return high==zz.high? (low<zz.low) : (high<zz.high); }
	bool operator >=(const ZZNumber &zz) const{ return high==zz.high? (low>=zz.low) : (high>zz.high); }
	bool operator <=(const ZZNumber &zz) const{ return high==zz.high? (low<=zz.low) : (high<zz.high); }
	ZZNumber &operator ++(){ if (++low >= 36){ low = 0; if (++high >= 36) high=0; } return *this; }
	ZZNumber &operator --(){ if (low-- == 0){ low = 35; if (high-- == 0) high=35; } return *this; }
	ZZNumber operator ++(int dummy){ ZZNumber zz = *this; ++*this; return zz; }
	ZZNumber operator --(int dummy){ ZZNumber zz = *this; --*this; return zz; }
	wxString to_string() const{ return wxString::Format("%c%c", _x(high), _x(low)); }
	bool from_string(const wxString &str){
		int h = _rx(str[0]), l = _rx(str[1]);
		if (h >= 36) return false;
		if (l >= 36) return false;
		high = h;
		low = l;
		return true;
	}
	// FF��`�p���\�b�h
	bool in_ff() const{ return (low|high)<16; }
	void increment_in_ff(){ if (++low >= 16){ low=0; if (++high >= 16) high=0; } }
	void decrement_in_ff(){ if (low-- == 0){ low=15; if (high-- == 0) high=15; } }
};



