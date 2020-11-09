#ifndef BMHELPER_CONFIGURATION_H
#define BMHELPER_CONFIGURATION_H

#include <wx/xml/xml.h>

class Configuration {

    wxXmlDocument doc;
    bool doc_loaded;

    wxXmlNode *GetLanguageNode();

public:
    Configuration() = default;
    virtual ~Configuration();

    void Init();
    bool IsLanguageSet();
    wxLanguage GetLanguage();
    void SetLanguage(wxLanguage lang);
};


#endif //BMHELPER_CONFIGURATION_H
