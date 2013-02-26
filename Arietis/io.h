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
 
class Serializer {
public:
    static bool     Serialize(ISerializable *obj, std::string &output);
    static bool     Deserialzie(ISerializable *obj, std::string &input);
private:
    Serializer();
};

#endif // __ARIETIS_IO_H__