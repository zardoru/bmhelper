
#include "smf_io.h"


int SmfEvent::read(const unsigned char *src, int &status){
	int data_size, var_size;
	int read_size = 0;
	int first = src[read_size];
	if (first & 0x80){
		status = first;
		read_size++;
	}
	channel = status & 0x0f;
	switch (status & 0xf0){
		case 0x80:
			type = SMF_EVENT_NOTE_OFF;
			note_off.nn = src[read_size++];
			note_off.v  = src[read_size++];
			break;
		case 0x90:
			type = SMF_EVENT_NOTE_ON;
			note_on.nn  = src[read_size++];
			note_on.v   = src[read_size++];
			if (note_on.v == 0){
				type = SMF_EVENT_NOTE_OFF;
			}
			break;
		case 0xA0:
			type = SMF_EVENT_PLKY_PRS;
			plky_prs.nn = src[read_size++];
			plky_prs.v  = src[read_size++];
			break;
		case 0xB0:
			type = SMF_EVENT_CTRL_CHG;
			ctrl_chg.ccn = src[read_size++];
			ctrl_chg.v   = src[read_size++];
			break;
		case 0xC0:
			type = SMF_EVENT_PROG_CHG;
			prog_chg.pn = src[read_size++];
			break;
		case 0xD0:
			type = SMF_EVENT_CHNL_PRS;
			chnl_prs.v  = src[read_size++];
			break;
		case 0xE0:
			type = SMF_EVENT_PTCH_BND;
			ptch_bnd.v  = src[read_size++];
			ptch_bnd.v |= (int)src[read_size++] << 7;
			break;
		case 0xF0:
			switch (channel){
				case 0x00:
					type = SMF_EVENT_SYSX_F0;
					data_size = SmfReadVarInt(&src[read_size], var_size);
					read_size += var_size;
					var_data.resize(data_size);
					for (int i=0; i<data_size; i++){
						var_data[i] = src[read_size++];
					}
					break;
				case 0x07:
					type = SMF_EVENT_SYSX_F7;
					data_size = SmfReadVarInt(&src[read_size], var_size);
					read_size += var_size;
					var_data.resize(data_size);
					for (int i=0; i<data_size; i++){
						var_data[i] = src[read_size++];
					}
					break;
				case 0x0F:
					type = SMF_EVENT_META;
					meta.type = src[read_size++];
					data_size = SmfReadVarInt(&src[read_size], var_size);
					read_size += var_size;
					var_data.resize(data_size);
					for (int i=0; i<data_size; i++){
						var_data[i] = src[read_size++];
					}
					break;
				default:
					return -1;
			}
			break;
		default:
			return -1;
	}
	return read_size;
}


bool SmfEvent::read(wxFile &src, int &status){
	int data_size, first;
	src.Read(&first, 1);
	if (first & 0x80){
		status = first;
	}else{
		src.Seek(-1, wxFromCurrent);
	}
	channel = status & 0x0f;
	switch (status & 0xf0){
		case 0x80:
			type = SMF_EVENT_NOTE_OFF;
			src.Read(&note_off.nn, 1);
			src.Read(&note_off.v, 1);
			break;
		case 0x90:
			type = SMF_EVENT_NOTE_ON;
			src.Read(&note_on.nn, 1);
			src.Read(&note_on.v, 1);
			break;
		case 0xA0:
			type = SMF_EVENT_PLKY_PRS;
			src.Read(&plky_prs.nn, 1);
			src.Read(&plky_prs.v, 1);
			break;
		case 0xB0:
			type = SMF_EVENT_CTRL_CHG;
			src.Read(&ctrl_chg.ccn, 1);
			src.Read(&ctrl_chg.v, 1);
			break;
		case 0xC0:
			type = SMF_EVENT_PROG_CHG;
			src.Read(&prog_chg.pn, 1);
			break;
		case 0xD0:
			type = SMF_EVENT_CHNL_PRS;
			src.Read(&chnl_prs.v, 1);
			break;
		case 0xE0:
			type = SMF_EVENT_PTCH_BND;
			{
				int lsb, msb;
				src.Read(&lsb, 1);
				src.Read(&msb, 1);
				ptch_bnd.v = lsb | (msb << 7);
			}
			break;
		case 0xF0:
			switch (channel){
				case 0x00:
					type = SMF_EVENT_SYSX_F0;
					data_size = SmfReadVarInt(src);
					var_data.resize(data_size);
					for (int i=0; i<data_size; i++){
						src.Read(&var_data[i], 1);
					}
					break;
				case 0x07:
					type = SMF_EVENT_SYSX_F7;
					data_size = SmfReadVarInt(src);
					var_data.resize(data_size);
					for (int i=0; i<data_size; i++){
						src.Read(&var_data[i], 1);
					}
					break;
				case 0x0F:
					type = SMF_EVENT_META;
					src.Read(&meta.type, 1);
					data_size = SmfReadVarInt(src);
					var_data.resize(data_size);
					for (int i=0; i<data_size; i++){
						src.Read(&var_data[i], 1);
					}
					break;
				default:
					return false;
			}
			break;
		default:
			return false;
	}
	return true;
}



int SmfEvent::write(unsigned char *dst, int &status) const{
	int written_size = 0;
	int new_status, var_size;
	switch (type){
		case SMF_EVENT_NOTE_OFF:
			new_status = 0x80 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = note_off.nn;
			dst[written_size++] = note_off.v;
			break;
		case SMF_EVENT_NOTE_ON:
			new_status = 0x90 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = note_on.nn;
			dst[written_size++] = note_on.v;
			break;
		case SMF_EVENT_PLKY_PRS:
			new_status = 0xA0 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = plky_prs.nn;
			dst[written_size++] = plky_prs.v;
			break;
		case SMF_EVENT_CTRL_CHG:
			new_status = 0xB0 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = ctrl_chg.ccn;
			dst[written_size++] = ctrl_chg.v;
			break;
		case SMF_EVENT_PROG_CHG:
			new_status = 0xC0 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = prog_chg.pn;
			break;
		case SMF_EVENT_CHNL_PRS:
			new_status = 0xD0 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = chnl_prs.v;
			break;
		case SMF_EVENT_PTCH_BND:
			new_status = 0xE0 | channel;
			if (new_status != status){
				status = new_status;
				dst[written_size++] = status;
			}
			dst[written_size++] = ptch_bnd.v & 0x007f;
			dst[written_size++] = (ptch_bnd.v & 0x3f80) >> 7;
			break;
		case SMF_EVENT_SYSX_F0:
			status = 0;				// ステータスクリア
			dst[written_size++] = 0xF0;
			SmfWriteVarInt(var_data.size(), &dst[written_size], var_size);
			written_size += var_size;
			for (size_t i=0; i<var_data.size(); i++){
				dst[written_size++] = var_data[i];
			}
			break;
		case SMF_EVENT_SYSX_F7:
			status = 0;				// ステータスクリア
			dst[written_size++] = 0xF7;
			SmfWriteVarInt(var_data.size(), &dst[written_size], var_size);
			written_size += var_size;
			for (size_t i=0; i<var_data.size(); i++){
				dst[written_size++] = var_data[i];
			}
			break;
		case SMF_EVENT_META:
			status = 0;				// ステータスクリア
			dst[written_size++] = 0xFF;
			dst[written_size++] = meta.type;
			SmfWriteVarInt(var_data.size(), &dst[written_size], var_size);
			written_size += var_size;
			for (size_t i=0; i<var_data.size(); i++){
				dst[written_size++] = var_data[i];
			}
			break;
		default:
			return -1;
	}
	return written_size;
}


bool SmfEvent::write(wxFile &dst, int &status) const{
	int new_status;
	switch (type){
		case SMF_EVENT_NOTE_OFF:
			new_status = 0x80 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			dst.Write(&note_off.nn, 1);
			dst.Write(&note_off.v, 1);
			break;
		case SMF_EVENT_NOTE_ON:
			new_status = 0x90 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			dst.Write(&note_on.nn, 1);
			dst.Write(&note_on.v, 1);
			break;
		case SMF_EVENT_PLKY_PRS:
			new_status = 0xA0 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			dst.Write(&plky_prs.nn, 1);
			dst.Write(&plky_prs.v, 1);
			break;
		case SMF_EVENT_CTRL_CHG:
			new_status = 0xB0 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			dst.Write(&ctrl_chg.ccn, 1);
			dst.Write(&ctrl_chg.v, 1);
			break;
		case SMF_EVENT_PROG_CHG:
			new_status = 0xC0 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			dst.Write(&prog_chg.pn, 1);
			break;
		case SMF_EVENT_CHNL_PRS:
			new_status = 0xD0 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			dst.Write(&chnl_prs.v, 1);
			break;
		case SMF_EVENT_PTCH_BND:
			new_status = 0xE0 | channel;
			if (new_status != status){
				status = new_status;
				dst.Write(&status, 1);
			}
			{
				int lsb, msb;
				lsb = ptch_bnd.v & 0x007f;
				msb = (ptch_bnd.v & 0x3f80) >> 7;
				dst.Write(&lsb, 1);
				dst.Write(&msb, 1);
			}
			break;
		case SMF_EVENT_SYSX_F0:
			status = 0;				// ステータスクリア
			new_status = 0xF0;
			dst.Write(&new_status, 1);
			SmfWriteVarInt(var_data.size(), dst);
			for (size_t i=0; i<var_data.size(); i++){
				dst.Write(&var_data[i], 1);
			}
			break;
		case SMF_EVENT_SYSX_F7:
			status = 0;				// ステータスクリア
			new_status = 0xF7;
			dst.Write(&new_status, 1);
			SmfWriteVarInt(var_data.size(), dst);
			for (size_t i=0; i<var_data.size(); i++){
				dst.Write(&var_data[i], 1);
			}
			break;
		case SMF_EVENT_META:
			status = 0;				// ステータスクリア
			new_status = 0xFF;
			dst.Write(&new_status, 1);
			dst.Write(&meta.type, 1);
			SmfWriteVarInt(var_data.size(), dst);
			for (size_t i=0; i<var_data.size(); i++){
				dst.Write(&var_data[i], 1);
			}
			break;
		default:
			return false;
	}
	return true;
}


