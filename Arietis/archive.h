#pragma once
 
#ifndef __ARIETIS_ARCHIVE_H__
#define __ARIETIS_ARCHIVE_H__
 
#include "Arietis.h"
#include "io.h"
#include "dbg/breakpoint.h"

class Archive : public ISerializable {
public:
    Archive();
    ~Archive();

    void        Serialize(Json::Value &root) override;
    void        Deserialize(Json::Value &root) override;

private:
    std::vector<Breakpoint>     m_breakpoints;
};
 
#endif // __ARIETIS_ARCHIVE_H__