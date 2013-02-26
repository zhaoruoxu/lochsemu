#pragma once
 
#ifndef __ARIETIS_DBG_BREAKPOINT_H__
#define __ARIETIS_DBG_BREAKPOINT_H__
 
#include "Arietis.h"
#include "io.h"

class Breakpoint : public ISerializable {
public:
    Breakpoint(u32 module, u32 offset, const std::string &desc);
    ~Breakpoint() {}

    void        Serialize(Json::Value &root) override;
    void        Deserialize(Json::Value &root) override;

private:
    u32         m_module;
    u32         m_offset;
    std::string m_desc;
};


 
#endif // __ARIETIS_DBG_BREAKPOINT_H__