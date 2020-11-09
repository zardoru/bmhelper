
#pragma once

#include "frame.h"
#include "Configuration.h"

class Application : public wxApp{
    wxLocale m_locale;
    Configuration cfg;
public:
	bool OnInit() override;
};


