#include "common.h"
#include "Configuration.h"

int Languages[] = {
        static_cast<int>(SupportedLanguages::DEFAULT),
        static_cast<int>(SupportedLanguages::ENGLISH),
        static_cast<int>(SupportedLanguages::SPANISH),
        static_cast<int>(SupportedLanguages::JAPANESE),
};

wxString LangNames[] = {
        "System",
        "English",
        wxString::FromUTF8("Español"),
        wxString::FromUTF8("日本語")
};

void Configuration::Init() {
    if (doc.Load("settings.xml")) {
        doc_loaded = true;
    } else {
        wxLogInfo("settings.xml does not exist. First run?");
        doc.SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, "bmhelper-config"));
    }
}

Configuration::~Configuration() {
    doc.Save("settings.xml");
}

wxXmlNode *Configuration::GetLanguageNode() {
    auto child = doc.GetRoot()->GetChildren();
    while (child) {
        if (child->GetName() == "language") {
            return child;
        }
        child = child->GetNext();
    }

    return nullptr;
}

wxLanguage Configuration::GetLanguage() {
    auto node = GetLanguageNode();
    if (node) {
        auto default_value = wxString::Format("%d", SupportedLanguages::DEFAULT);
        auto attr = node->GetAttribute("value", default_value);
        auto lang = wxStrtol(attr, nullptr, 10);
        return (wxLanguage)lang;
    }

    return (wxLanguage)SupportedLanguages::DEFAULT;
}

bool Configuration::IsLanguageSet() {
    return GetLanguageNode() != nullptr;
}

void Configuration::SetLanguage(wxLanguage lang) {
    auto node = GetLanguageNode();
    if (!node) {
        node = new wxXmlNode(doc.GetRoot(), wxXML_ELEMENT_NODE, "language");
    }

    if (node->HasAttribute("value"))
        node->DeleteAttribute("value");

    node->AddAttribute("value", wxString::Format("%d", (int)lang));
}
