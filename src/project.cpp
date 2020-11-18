
#include "project.h"
#include <wx/xml/xml.h>

const char *project_file_ext = "bmh";
const char *midi_file_filter = "MIDI File(*.mid;*.midi;*.smf)|*.mid;*.midi;*.smf";
const char *project_file_filter = "BMHelper project(*.bmh)|*.bmh";


Project::Project(const wxString &_filename)
        : titled(false), changed(false), observer(0) {
    wxFileName fn(_filename);
    wxString ext = fn.GetExt();
    if (!ext.CmpNoCase(_("mid")) || !ext.CmpNoCase(_("midi")) || !ext.CmpNoCase(_("smf"))) {
        /// midiファイルから新規プロジェクト作成
        titled = false;
        changed = true;
        if (!source.load_smf(_filename)) {
            throw 0;
        }
        filename = fn;
        filename.SetExt(project_file_ext);
    } else if (!ext.CmpNoCase(project_file_ext)) {
		// 既存のプロジェクトを開く
        filename = _filename;
        titled = true;
        changed = false;
        TreeDocument document;
        document.load(_filename);
        if (!read_tree(document.get_root())) throw 0;
    } else {
		// 未知の拡張子
        throw 0;
    }
}

Project::~Project() {
    ClearDivisions();
}

void Project::init() {
    ClearDivisions();
}

void Project::SetChangeFlag(bool f) {
    if (changed != f) {
        changed = f;
        if (observer) observer->ProjectChangeFlagChanged();
    }
}

bool Project::Titled() const {
    return titled;
}

bool Project::Changed() const {
    return changed;
}

bool Project::Save() {
    if (!titled) return false;
    {
        TreeDocument document;
        if (!write_tree(document.get_root())) return false;
        document.save(filename.GetFullPath());
    }
    SetChangeFlag(false);
    return true;
}

bool Project::SaveAs(const wxString &_new_filename) {
    filename = wxFileName(_new_filename);
    titled = true;
    bool bRet = Save();
    if (observer) observer->ProjectFileNameChanged();
    return bRet;
}


ProjectObserver *Project::SetObserver(ProjectObserver *new_obs) {
    ProjectObserver *old = observer;
    observer = new_obs;
    return old;
}

wxString Project::GetFileName() const {
    return filename.GetFullPath();
}

wxString Project::GetFileTitle() const {
    return filename.GetFullName();
}

wxString Project::GetTitle() const {
    return filename.GetName();
}


void Project::ClearDivisions() {
    if (divisions.empty()) return;
    divisions.clear();
    changed = true;
}

size_t Project::CreateDivision(const DivisionSetting &setting) {
    size_t n = divisions.size();
    divisions.emplace_back(Division(this, source, setting));
    SetChangeFlag(true);
    return n;
}

void Project::EraseDivision(size_t index) {
    int n = 0;
    for (auto i = divisions.begin(); i != divisions.end(); i++, n++) {
        if (n == index) {
            divisions.erase(i);
            SetChangeFlag(true);
            break;
        }
    }
}

//-----------------------------------------------------------------------


static const NodeName _BMHP_ = StringToNodeName("BMHP");
static const NodeName _Src_ = StringToNodeName("Src ");
static const NodeName _Div_ = StringToNodeName("Div ");


bool Project::read_tree(TreeNode &node) {
    try {
        if (node.get_name() != _BMHP_) throw 0;
        for (auto &i : node) {
            if (i.get_name() == _Src_) {
                if (!source.read_tree(i)) throw 1;
            } else if (i.get_name() == _Div_) {
                divisions.emplace_back(Division(this));
                Division &div = divisions.back();
                if (!div.read_tree(i)) throw 2;
            }
        }
    } catch (...) {
        return false;
    }
    return true;
}

bool Project::write_tree(TreeNode &node) {
    try {
        node.set_name(_BMHP_);
        node.push_back(TreeNode(_Src_));
        if (!source.write_tree(node.back())) throw 0;
        for (auto &division : divisions) {
            node.push_back(TreeNode(_Div_));
            if (!division.write_tree(node.back())) throw 1;
        }
    } catch (...) {
        return false;
    }
    return true;
}

void Project::SetNewSource(const wxString &filename) {
    source.load_smf(filename);
}




