#include "stdafx.h"
#include "protocol/analyzer.h"

class Tainter : public ProtocolAnalyzer {
public:
    Tainter() : ProtocolAnalyzer(100, "Tainter", WinapiPostCallHandler)
    {

    }

    void        Serialize(Json::Value &root) const override
    {
        ProtocolAnalyzer::Serialize(root);
    }

    void        Deserialize(Json::Value &root) override
    {
        ProtocolAnalyzer::Deserialize(root);
    }

    void        OnWinapiPostCall(WinapiPostCallEvent &event) override 
    {
        //LxWarning("Winapi post call: %d\n", event.ApiIndex);
    }
};

Tainter t;