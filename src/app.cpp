
#include "app.h"

const char* app_name = "Be-Music Helper";
const char* app_version = "beta 5 (wyrmin)";

bool Application::OnInit(){

    if (!wxApp::OnInit())
        return false;

    wxLog::SetActiveTarget(new wxLogStderr());

    wxLogInfo("Starting bmhelper");

    cfg.Init();

    int lang = -1;
    if (!cfg.IsLanguageSet()) {
        int idx = wxGetSingleChoiceIndex(
                _("Please choose language:"),
                _("Language"),
                (int)SupportedLanguages::LANG_COUNT,
                LangNames, 0
        );

        lang = Languages[idx];
        cfg.SetLanguage((wxLanguage)lang);
    }

    lang = cfg.GetLanguage();

    if (!m_locale.Init(lang, wxLOCALE_DONT_LOAD_DEFAULT)) {
        wxLogWarning("unsupported language...");
    }

    wxLocale::AddCatalogLookupPathPrefix(".");

    const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(lang);
    if (!m_locale.AddCatalog("bmhelper"))
    {
        wxLogError(_("Couldn't find/load the 'bmhelper' catalog for locale '%s'."),
                   pInfo ? pInfo->GetLocaleName() : _("unknown"));
    }

    wxLogInfo("locale: %s (%s)", pInfo->GetLocaleName(), pInfo->Description);

	auto frame = new FrameWindow(wxPoint(wxDefaultCoord, wxDefaultCoord), wxSize(800, 600));
	auto args = std::vector<wxString>(argc-1);

	for (int i = 1; i < argc; i++) {
	    args.emplace_back(argv[i]);
	}

	frame->OpenFiles(argc-1, args);
	SetTopWindow(frame);
	frame->Show();
	return true;
}



wxIMPLEMENT_APP(Application);