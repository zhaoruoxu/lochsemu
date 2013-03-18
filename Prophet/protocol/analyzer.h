#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZER_H__
#define __PROPHET_PROTOCOL_ANALYZER_H__

#include "prophet.h"
#include "event.h"
#include "utilities.h"


class ProtocolAnalyzer : public ISerializable {
public:
    ProtocolAnalyzer(Protocol *protocol, const std::string &name, uint handlerFlags);
    virtual ~ProtocolAnalyzer();

    std::string     GetName() const { return m_name; }
    bool            IsEnabled() const { return m_enabled; }
    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool            HasHandlerFlag(EventHandlerFlag f) const { return (m_handlerFlags & f) != 0; }
    Protocol *          GetProtocol() { return m_protocol; }
    const Protocol *    GetProtocol() const { return m_protocol; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;

    virtual void    Initialize() {}
    virtual void    OnPreExecute        (PreExecuteEvent        &event) {}
    virtual void    OnPostExecute       (PostExecuteEvent       &event) {}
    virtual void    OnMemRead           (MemReadEvent           &event) {}
    virtual void    OnMemWrite          (MemWriteEvent          &event) {}
    virtual void    OnProcessPreRun     (ProcessPreRunEvent     &event) {}
    virtual void    OnProcessPostRun    (ProcessPostRunEvent    &event) {}
    virtual void    OnProcessPreLoad    (ProcessPreLoadEvent    &event) {}
    virtual void    OnProcessPostLoad   (ProcessPostLoadEvent   &event) {}
    virtual void    OnWinapiPreCall     (WinapiPreCallEvent     &event) {}
    virtual void    OnWinapiPostCall    (WinapiPostCallEvent    &event) {}
    virtual void    OnMessageBegin      (MessageBeginEvent      &event) {}
    virtual void    OnMessageEnd        (MessageEndEvent        &event) {}

public:
    //static bool     Compare(const ProtocolAnalyzer *lhs, const ProtocolAnalyzer *rhs);

private:
    std::string     m_name;
    bool            m_enabled;
    Protocol *      m_protocol;
    uint            m_handlerFlags;
};

#endif // __PROPHET_PROTOCOL_ANALYZER_H__