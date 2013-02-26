#pragma once
 
#ifndef __ARIETIS_ARCHIVE_H__
#define __ARIETIS_ARCHIVE_H__
 
#include "Arietis.h"
#include "io.h"
#include "parallel.h"
#include "dbg/breakpoint.h"

struct Archive : public ISerializable, public MutexSyncObject {

    Archive();
    ~Archive();

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;
    //void        Lock() const { m_mutex.Wait(); }
    //void        Unlock() const { m_mutex.Release(); }

    std::vector<Breakpoint>     Breakpoints;

private:
   // Mutex       m_mutex;
};
 
#endif // __ARIETIS_ARCHIVE_H__