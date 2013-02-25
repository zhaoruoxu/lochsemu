#pragma once
 
#ifndef __ARIETIS_IO_H__
#define __ARIETIS_IO_H__
 
#include "Arietis.h"

class ISerializable {
public:
    virtual ~ISerializable() {}
    virtual void    Serialize(Json::Value &root) = 0;
    virtual void    Deserialize(Json::Value &root) = 0;
};
 
#endif // __ARIETIS_IO_H__