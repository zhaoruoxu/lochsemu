#pragma once
 
#ifndef __PROPHET_ARCHIVE_H__
#define __PROPHET_ARCHIVE_H__
 
#include "Prophet.h"
#include "utilities.h"
#include "parallel.h"

struct Archive : public ISerializable, public MutexSyncObject {
    Archive();
    ~Archive();

    void        AddObject(const std::string &name, ISerializable *obj) {
        m_objects[name] = obj;
    }
    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

private:
    std::map<std::string, ISerializable *>    m_objects;
};
 
#endif // __PROPHET_ARCHIVE_H__