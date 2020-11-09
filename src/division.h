
#pragma once

#include "midi_data.h"
#include <vector>

class Project;


struct DivisionSetting{

	wxString name;

	// midi setting
	bool src_copy;				// ���̃V�[�P���X�����̂܂܎c��(true�̏ꍇ���̍��ڂ͖���)
	size_t head_margin;			// ���̗]��
	size_t min_interval;		// �m�[�g�Ԃ̍ŏ��Ԋu(�m�[�g�I�t���玟�̃m�[�g�I���܂�)
	enum SortType{
		SORT_NONE		= 0,
		SORT_NN_GATE_V	= 1,	// �m�[�gNo. / ���� / �x���V�e�B
		SORT_NN_V_GATE	= 2,
		SORT_GATE_NN_V	= 3,
		SORT_GATE_V_NN	= 4,
		SORT_V_NN_GATE	= 5,
		SORT_V_GATE_NN	= 6
	} sort_type;
	size_t gate_threshold;		// �m�[�g�𓯈ꎋ���钷����臒l
	size_t velocity_threshold;

	// definition setting
	bool zz_definition;			// ZZ��`��L���ɂ���
	bool ml_definition;			// ���d��`��L���ɂ���
	ZZNumber start_definition;	// �J�n��`�ԍ�
	int ml_threshold;			// ���d��`���邩�ǂ����̃m�[�g�Ԋu��臒l(���l���ݒ�B���x�̏ꍇ�͑��d��`���Ȃ�)

};



struct Definition{
	ZZNumber zz;
	int div;

	Definition(){}
	Definition(ZZNumber _zz, int _div) : zz(_zz), div(_div){}
};



class Division : public MidiData{

public:
	typedef std::vector<Definition> DefinitionsVector;

private:
	Project *project;
	wxString name;
	std::vector<int> src2def;
	DefinitionsVector definitions;
	int head_margin;
	bool zz_enabled;

	//void _divide_notes(std::vector<int> &src2div, std::vector<ReferredNote> &temp_divs, ThresholdSetting &thresholds);
	//void _sort_notes(std::vector<ReferredNote> &temp_divs, DivisionSetting::SortType sort_type);

public:
	Division(Project *_project);
	Division(Project *_project, MidiData &src, const DivisionSetting &setting);
	void init(){
		//MidiData::init();
		name.clear();
		src2def.clear();
		definitions.clear();
	}

	virtual bool read_tree(TreeNode &node);
	virtual bool write_tree(TreeNode &node);

	const wxString &get_name(){ return name; }
	void set_name(const wxString &_n){ name = _n; }

	size_t definitions_count() const{ return definitions.size(); }
	Definition &definition(size_t i){ return definitions[i]; }

	//void enable_zz_definition(bool f=true);
	//bool shift_definition_left(int bound_def_num=-1);
	//bool shift_definition_right(int bound_def_num=0);
	bool head_margin_exists(){ return head_margin != 0; }

	wxString get_div_sequence_data() const;						// �ؒf�ʒu�̃V�[�P���X�f�[�^
	wxString get_bms_sequence_data(const MidiData &src) const;	// BMS�V�[�P���X�f�[�^
	
	bool is_zz_enabled(){ return zz_enabled; }
	void def_transpose_up();
	void def_transpose_down();
	void def_transpose_to(ZZNumber nbegin);
};




