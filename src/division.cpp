
#include "division.h"
#include "project.h"
#include <algorithm>
#include <map>
#include <wx/xml/xml.h>

/*
	クッソ効率の悪い分割の仕方してるけど、これくらいのデータ量なら大丈夫でしょ
	気になる暇人がいたら適当に改良してちょ
*/


struct ThresholdSetting {
    int gate;
    int vel;
};

struct BasicNote {
    int nn;
    int gate;
    int vel;

    bool accepts(const BasicNote &note, const ThresholdSetting &thresholds) const {
        return (nn == note.nn) && (abs(gate - note.gate) <= thresholds.gate) && (abs(vel - note.vel) <= thresholds.vel);
    }
};

struct IndexedNote : public BasicNote {
    int index;

    bool operator>(const IndexedNote &r) const {
        return nn == r.nn ? (gate == r.gate ? vel > r.vel : gate > r.gate) : nn > r.nn;
    }

    bool operator<(const IndexedNote &r) const {
        return nn == r.nn ? (gate == r.gate ? vel < r.vel : gate < r.gate) : nn < r.nn;
    }
};

struct ReferredNote : public BasicNote {
    std::vector<int> referrers;
};

class NotesCluster {
    std::vector<IndexedNote> notes;

    static bool ByGate(const IndexedNote &l, const IndexedNote &r) {    // ��r(<)
        return l.gate < r.gate;
    }

    static bool ByVel(const IndexedNote &l, const IndexedNote &r) {    // ��r(<)
        return l.vel < r.vel;
    }

public:
    NotesCluster() = default;

    NotesCluster(const IndexedNote &note) : notes(1, note) {}

    bool accept(const IndexedNote &note, const ThresholdSetting &thresholds) {
        for (auto &i: notes) {
            if (i.accepts(note, thresholds)) {
                notes.push_back(note);
                return true;
            }
        }
        return false;
    }

    void divide(std::vector<int> &src2div, std::vector<ReferredNote> &divs, const ThresholdSetting &thresholds) {
        int gate_min = min_element(notes.begin(), notes.end(), ByGate)->gate;
        int gate_max = max_element(notes.begin(), notes.end(), ByGate)->gate + 1;
        int vel_min = min_element(notes.begin(), notes.end(), ByVel)->vel;
        int vel_max = max_element(notes.begin(), notes.end(), ByVel)->vel + 1;
        int gate_div = (gate_max - gate_min + thresholds.gate) / (thresholds.gate + 1);
        int vel_div = (vel_max - vel_min + thresholds.vel) / (thresholds.vel + 1);
        int gt = gate_min, gt_nxt;
        for (int ig = 0; ig < gate_div; ig++) {
            gt_nxt = gate_min + (ig + 1) * abs(gate_max - gate_min) / gate_div;
            int vl = vel_min, vl_nxt;
            for (int iv = 0; iv < vel_div; iv++) {
                vl_nxt = vel_min + (iv + 1) * abs(vel_max - vel_min) / vel_div;
                int c = 0;
                int div_index = divs.size();
                ReferredNote div_note;
                div_note.nn = 0;
                div_note.gate = 0;
                div_note.vel = 0;
                for (auto &note : notes) {
                    if (note.gate >= gt && note.gate < gt_nxt && note.vel >= vl && note.vel < vl_nxt) {
                        src2div[note.index] = div_index;
                        div_note.referrers.push_back(note.index);
                        div_note.nn += note.nn;
                        div_note.gate += note.gate;
                        div_note.vel += note.vel;
                        c++;
                    }
                }
                if (c) {
                    div_note.nn /= c;
                    div_note.gate /= c;
                    div_note.vel /= c;
                    divs.push_back(div_note);
                }
                vl = vl_nxt;
            }
            gt = gt_nxt;
        }
    }
};

typedef int (*CompareNoteFunc)(const ReferredNote &, const ReferredNote &);

static int CompareNote_N_Asc(const ReferredNote &l, const ReferredNote &r) {
    return l.nn - r.nn;
}

static int CompareNote_G_Desc(const ReferredNote &l, const ReferredNote &r) {
    return r.gate - l.gate;
}

static int CompareNote_V_Desc(const ReferredNote &l, const ReferredNote &r) {
    return r.vel - l.vel;
}

#define COMPARE_NOTE(fa, l, r) \
    ((((fa)[0](l,r))? ((fa)[0](l,r)) : ( ((fa)[1](l,r))? ((fa)[1](l,r)) : ((fa)[2](l,r)) ) )>0)

Division::Division(Project *_project) : project(_project) {}

static void divide_notes(
        MidiData &src,
        std::vector<int> &src2div,
        std::vector<ReferredNote> &temp_divs,
        ThresholdSetting &thresholds
) {
    std::vector<NotesCluster> clusters;
    for (size_t i = 0; i != src.notes_count(); i++) {
        IndexedNote note{};
        note.index = i;
        note.nn = src.notes(i).note_num;
        note.gate = src.notes(i).gate;
        note.vel = src.notes(i).velocity;
        bool accepted = false;
        for (auto &ic: clusters) {
            if (ic.accept(note, thresholds)) {
                accepted = true;
                break;
            }
        }
        if (!accepted) {
            clusters.emplace_back(note);
        }
    }
    for (auto &cluster : clusters) {
        cluster.divide(src2div, temp_divs, thresholds);
    }
}

static void sort_notes(
        std::vector<ReferredNote> &temp_divs,
        std::vector<int> &src2div,
        DivisionSetting::SortType sort_type
) {
    if (sort_type == DivisionSetting::SORT_NONE) return;
    CompareNoteFunc faa[][3] = {
            {CompareNote_N_Asc, CompareNote_G_Desc,  CompareNote_V_Desc},
            {CompareNote_N_Asc, CompareNote_V_Desc,  CompareNote_G_Desc},
            {CompareNote_G_Desc, CompareNote_N_Asc,  CompareNote_V_Desc},
            {CompareNote_G_Desc, CompareNote_V_Desc, CompareNote_N_Asc},
            {CompareNote_V_Desc, CompareNote_N_Asc,  CompareNote_G_Desc},
            {CompareNote_V_Desc, CompareNote_G_Desc, CompareNote_N_Asc},
    };
    CompareNoteFunc *fa = faa[(int) sort_type - 1];
    for (size_t i1 = 0; i1 < temp_divs.size() - 1; i1++) {
        for (size_t i2 = i1 + 1; i2 < temp_divs.size(); i2++) {
            if (COMPARE_NOTE(fa, temp_divs[i1], temp_divs[i2])) {
                ReferredNote note = temp_divs[i1];
                temp_divs[i1] = temp_divs[i2];
                temp_divs[i2] = note;
                for (size_t ir = 0; ir < temp_divs[i1].referrers.size(); ir++)
                    src2div[temp_divs[i1].referrers[ir]] = i1;
                for (size_t ir = 0; ir < temp_divs[i2].referrers.size(); ir++)
                    src2div[temp_divs[i2].referrers[ir]] = i2;
            }
        }
    }
}

Division::Division(Project *_project, MidiData &src, const DivisionSetting &setting)
        : MidiData(src.get_quantize()), project(_project), name(setting.name), zz_enabled(setting.zz_definition) {
    divide_from_data(src, setting);
}

void Division::divide_from_data(MidiData &src, const DivisionSetting &setting, bool copy) {
    if (copy)
        src_data = src; // store a copy of the src data...
    else {
        auto q = quantize;
        init(); /* clean ourselves up */
        quantize = q;
    }

    name = setting.name;

    std::vector<ReferredNote> temp_divs;
    std::vector<int> src2div(src.notes_count());
    src2def.resize(src.notes_count());

    if (setting.src_copy) {
        // srcをtemp_divsにコピー
        temp_divs.resize(src.notes_count());
        for (size_t i = 0; i != src.notes_count(); i++) {
            ReferredNote &note = temp_divs[i];
            note.nn = src.notes(i).note_num;
            note.gate = src.notes(i).gate;
            note.vel = src.notes(i).velocity;
            note.referrers.push_back(i);
            src2div[i] = i;
        }

        // Midiデータにそのまま配置
        for (size_t i = 0; i < temp_divs.size(); i++) {
            note_push_back(
                    MidiNoteEvent(src.notes(i).position + setting.head_margin, temp_divs[i].gate, temp_divs[i].nn,
                                  temp_divs[i].vel));
        }

        if (src.notes_count() > 0) head_margin = src.notes(0).position + setting.head_margin;
    } else {
        // 分割
        ThresholdSetting thresholds {
            static_cast<int>(setting.gate_threshold),
            static_cast<int>(setting.velocity_threshold)
        };

        divide_notes(src, src2div, temp_divs, thresholds);

        // ソート
        sort_notes(temp_divs, src2div, setting.sort_type);

        // Midiデータに流し込み
        int position = setting.head_margin;
        for (auto & temp_div : temp_divs) {
            note_push_back(MidiNoteEvent(position, temp_div.gate, temp_div.nn, temp_div.vel));
            position += setting.min_interval + temp_div.gate;
            int b = position % get_quantize();
            if (b) position += get_quantize() - b;
        }

        head_margin = setting.head_margin;
    }

    // 定義の作成(多重定義が無い場合はdivと1体1対応)
    ZZNumber zz = setting.start_definition;
    if (!setting.zz_definition) if (!zz.in_ff()) zz.increment_in_ff();
    for (size_t i = 0; i < temp_divs.size(); i++) {
        // 必要な定義数を調べる
        size_t mln;
        if (setting.ml_definition) {
            for (mln = 1; mln <= temp_divs[i].referrers.size(); mln++) {
                int mint = INT_MAX;
                for (size_t b = 0; b < mln; b++) {
                    int pos = src.notes(temp_divs[i].referrers[b]).position;
                    int end = pos + src.notes(temp_divs[i].referrers[b]).gate;
                    for (size_t ir = b; ir < temp_divs[i].referrers.size() - mln; ir += mln) {
                        int next_pos = src.notes(temp_divs[i].referrers[ir + mln]).position;
                        int next_end = next_pos + src.notes(temp_divs[i].referrers[ir + mln]).gate;
                        if (next_pos - end < mint) {
                            mint = next_pos - end;
                        }
                        pos = next_pos;
                        end = next_end;
                    }
                }
                if (mint >= setting.ml_threshold) break;
            }
        } else {
            mln = 1;    // 多重定義しない
        }

        size_t id_i = definitions.size();
        for (size_t b = 0; b < mln; b++) {
            definitions.push_back(Definition(zz, i));
            if (setting.zz_definition) zz++; else zz.increment_in_ff();
        }

        size_t b = 0;
        for (int referrer : temp_divs[i].referrers) {
            src2def[referrer] = id_i + b;
            if (++b == mln) b = 0;
        }
    }
}


void Division::def_transpose_up() {
    //if (definitions.begin()->zz == ZZNumber(1)) return;
    if (zz_enabled) {
        for (auto & definition : definitions) {
            definition.zz--;
        }
    } else {
        for (auto & definition : definitions) {
            definition.zz.decrement_in_ff();
        }
    }
}

void Division::def_transpose_down() {
    if (zz_enabled) {
        //if (definitions.back().zz == ZZNumber(35,35)) return;
        for (auto & definition : definitions) {
            definition.zz++;
        }
    } else {
        //if (definitions.back().zz == ZZNumber(15,15)) return;
        for (auto & definition : definitions) {
            definition.zz.increment_in_ff();
        }
    }
}

void Division::def_transpose_to(ZZNumber nbegin) {
    if (definitions.empty()) return;
    if (zz_enabled) {
        int diff = int(nbegin) - int(definitions[0].zz);
        for (auto &definition : definitions) {
            int tmp = int(definition.zz) + diff;
            if (tmp < 0) tmp += 36 * 36;
            if (tmp >= 36 * 36) tmp -= 36 * 36;
            definition.zz = (unsigned) tmp;
        }
    } else {
        // �߂�ǂ��̂ŏ_��̂Ȃ���������
        for (auto & definition : definitions) {
            definition.zz = nbegin;
            nbegin.increment_in_ff();
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
//
//	コピペ用シーケンスデータの作成
// static const size_t parts_per_beat = 16 * 9 * 5 * 7; /* 2^4, 3^2, 5, 7 */
static const size_t bmse_quantize = 192 / 4;

wxString Division::get_div_sequence_data() const {
    wxString data(_("BMSE ClipBoard Object Data Format\r\n"));
    for (size_t i = 0; i < notes_count(); i++) {
        auto d = notes(i).position * bmse_quantize / get_quantize();
        data += wxString::Format(_("011%08zu1\r\n"), d);
    }
    return data;
}


wxString Division::get_bms_sequence_data(const MidiData &src) const {
    wxString data(_("BMSE ClipBoard Object Data Format\r\n"));
    int lane;
    std::map<int, int> lanes;
    for (size_t i = 0; i < src.notes_count(); i++) {
        ZZNumber zz = definitions[src2def[i]].zz;
        int position = src.notes(i).position * bmse_quantize / get_quantize();
        if (lanes.find(position) == lanes.end()) {
            lane = lanes[position] = 1;
        } else {
            lane = ++lanes[position];
        }
        auto t = static_cast<unsigned int>(zz);
        data += wxString::Format(_("1%02d%08d%d\r\n"), lane, position, t);
    }
    return data;
}




//////////////////////////////////////////////////////////////////////////////
//
//	Tree IO

static const NodeName _Midi_ = StringToNodeName("Midi");
static const NodeName _name_ = StringToNodeName("name");
static const NodeName _s2df_ = StringToNodeName("s2df");
static const NodeName _dfnt_ = StringToNodeName("dfnt");
static const NodeName _zzen_ = StringToNodeName("zzen");
static const NodeName _srcd_ = StringToNodeName("Srcd"); /* source data (first letter caps = list) */

struct BmsOffsetData {
    bool zz_enabled;
};

bool Division::read_tree(TreeNode &node) {
    init();
    for (auto &sub : node) {
        if (sub.get_name() == _Midi_) {
            if (!MidiData::read_tree(sub)) return false;
        } else if (sub.get_name() == _name_) {
            char *temp = new char[sub.get_data_size() + 1];
            if (sub.get_data_size()) sub.get_data(temp, sub.get_data_size());
            temp[sub.get_data_size()] = '\0';
            name = temp;
        } else if (sub.get_name() == _s2df_) {
            size_t size = sub.get_data_size() / sizeof(int);
            src2def.resize(size);
            int *temp = new int[size + 1];
            sub.get_data(temp, (size + 1) * (sizeof(int)));
            for (size_t i = 0; i < size; i++) src2def[i] = temp[i];
            delete[] temp;
        } else if (sub.get_name() == _dfnt_) {
            Definition d;
            if (sub.get_data_size() == sizeof(Definition)) {
                sub.get_data(&d, sizeof(Definition));
                definitions.emplace_back(d);
            }
        } else if (sub.get_name() == _zzen_) {
            BmsOffsetData data{};
            if (sub.get_data_size() == sizeof(BmsOffsetData)) {
                sub.get_data(&data, sizeof(BmsOffsetData));
                zz_enabled = data.zz_enabled;
            }
        } else if (sub.get_name() == _srcd_) {
            src_data.read_tree(sub);
        }
    }
    return true;
}



bool Division::write_tree(TreeNode &node) {
    node.push_back(_Midi_);
    if (!MidiData::write_tree(node.back())) return false;
    node.push_back(_name_);
    node.back().set_data(name.GetData(), name.size());
    {
        node.push_back(_s2df_);
        int *temp = new int[src2def.size()];
        for (size_t i = 0; i < src2def.size(); i++) temp[i] = src2def[i];
        node.back().set_data(temp, src2def.size() * (sizeof(int)));
        delete[] temp;
    }
    for (auto &definition : definitions) {
        node.push_back(_dfnt_);
        node.back().set_data(&definition, sizeof(Definition));
    }
    /* zz_enabled wasn't being saved... */
    auto offset_data = BmsOffsetData {
        zz_enabled,
    };

    node.push_back(_zzen_);
    node.back().set_data(&offset_data, sizeof(BmsOffsetData));

    node.push_back(_srcd_);
    src_data.write_tree(node.back());

    return true;
}

void Division::change_division_settings(const DivisionSetting &setting) {
    divide_from_data(src_data, setting, false);
}


DivisionSetting::DivisionSetting(const wxString &name, size_t quantize) {
    this->name = name;
    src_copy = false;
    head_margin = 0;
    min_interval = quantize*1;
    sort_type = DivisionSetting::SORT_NN_GATE_V;
    gate_threshold = quantize/8;
    velocity_threshold = 5;
    zz_definition = true;
    ml_definition = true;
    start_definition = 1;
    ml_threshold = quantize/2;
}
