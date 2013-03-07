#pragma once
 
#ifndef __PROPHET__PROTOCOL_H__
#define __PROPHET__PROTOCOL_H__
 
#include "Prophet.h"
#include "utilities.h"
#include "pretaint.h"

class Protocol : public ISerializable {
public:
    Protocol(ProEngine *engine);
    ~Protocol();

    void        Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool        IsEnabled() const { return m_enabled; }
    
    void        Initialize();
    void        OnWinapiPostCall(WinapiPostCallEvent &event);

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

private:
    ProEngine * m_engine;

    PreTainter  m_pretaint;

    bool        m_enabled;
};
 
#endif // __PROPHET__PROTOCOL_H__