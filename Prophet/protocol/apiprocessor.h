#pragma once
 
#ifndef __PROPHET_PROTOCOL_CONTROLLER_H__
#define __PROPHET_PROTOCOL_CONTROLLER_H__

#include "prophet.h"
#include "utilities.h"
#include "event.h"

class ApiProcessor : public ISerializable {
public:
    ApiProcessor(Protocol *protocol);
    ~ApiProcessor();

    void    Initialize();
    void    Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool    IsEnabled() const { return m_enabled; }

    void    OnWinapiPreCall(WinapiPreCallEvent &event);
    void    OnWinapiPostCall(WinapiPostCallEvent &event);

    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;

private:
    void    SerializeApi(Json::Value &root, int index, bool isPreCallApi) const;
    void    DeserializeApi(Json::Value &root, bool isPreCallApi);

private:
    Protocol *      m_protocol;
    TaintEngine *   m_taint;
    bool    m_enabled;

private:
    static const int    ApiCountMax = 4096;
    typedef     void (ApiProcessor::*WinapiHandler)(const Processor *cpu);

    void    InitializeDefaultHandlers();
    void    RegisterHandlerPreCall (const char *dllName, const char *apiName, WinapiHandler h, bool enabled);
    void    RegisterHandlerPostCall(const char *dllName, const char *apiName, WinapiHandler h, bool enabled);
    
    WinapiHandler   m_handlerPreCall[ApiCountMax];
    WinapiHandler   m_handlerPostCall[ApiCountMax];
    bool            m_isHandlerEnabledPreCall[ApiCountMax];
    bool            m_isHandlerEnabledPostCall[ApiCountMax];

private:
#define DECLARE_HANDLER(x)  void    x(const Processor *cpu);
    DECLARE_HANDLER(Handler_send);
    DECLARE_HANDLER(Handler_recv);
    DECLARE_HANDLER(Handler_ReadFile);
    DECLARE_HANDLER(Handler_WriteFile);

#undef DECLARE_HANDLER
};

#endif // __PROPHET_PROTOCOL_CONTROLLER_H__