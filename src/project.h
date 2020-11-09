
#pragma once

#include "common.h"
#include <wx/filename.h>
#include <string>
#include <vector>
#include "midi_data.h"
#include "division.h"


class ProjectObserver{
public:
	virtual void ProjectFileNameChanged(){}
	virtual void ProjectChangeFlagChanged(){}
};



class Project{

public:
	typedef std::vector<Division> DivisionsVector;

private:
	wxFileName filename;
	bool titled;
	bool changed;
	MidiData source;
	DivisionsVector divisions;

	ProjectObserver *observer;

	Project(const Project &src);
	bool read_tree(TreeNode &node);
	bool write_tree(TreeNode &node);

public:
	Project(const wxString &_filename);
	~Project();
	void init();
	bool save_project(const wxString &filename) const;

	void SetChangeFlag(bool f=true);
	bool Titled() const;
	bool Changed() const;
	bool Save();
	bool SaveAs(const wxString &_new_filename);

	ProjectObserver *SetObserver(ProjectObserver *new_obs);
	wxString GetFileName() const;
	wxString GetFileTitle() const;
	wxString GetTitle() const;
	MidiData &GetSource(){ return source; }

	void ClearDivisions();
	size_t CreateDivision(const DivisionSetting &setting);
	void EraseDivision(size_t i);
	size_t GetDivisionsCount() const{ return divisions.size(); }
	Division &GetDivision(size_t i){ return *divisions[i]; }

};


