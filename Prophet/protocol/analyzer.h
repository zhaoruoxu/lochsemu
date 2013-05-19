#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZER_H__
#define __PROPHET_PROTOCOL_ANALYZER_H__

#include "prophet.h"
#include "event.h"
#include "utilities.h"
#include "apiprocessor.h"
#include "protocol.h"

class TraceAnalyzer {
public:
    virtual void    Reset() {}
    virtual void    OnExecuteTrace(ExecuteTraceEvent &event) = 0;
    virtual void    OnComplete() {};
};

class ProtocolAnalyzer : public ISerializable {
public:
    ProtocolAnalyzer(Protocol *protocol, const std::string &name);
    virtual ~ProtocolAnalyzer();

    std::string     GetName() const { return m_name; }
    bool            IsEnabled() const { return m_enabled; }
    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    //bool            HasHandlerFlag(EventHandlerFlag f) const { return (m_handlerFlags & f) != 0; }
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

protected:
    Protocol *      m_protocol;

private:
    std::string     m_name;
    bool            m_enabled;
    //uint            m_handlerFlags;
};

#endif // __PROPHET_PROTOCOL_ANALYZER_H__