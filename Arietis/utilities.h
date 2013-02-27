#pragma once
 
#ifndef __ARIETIS_UTILITIES_H__
#define __ARIETIS_UTILITIES_H__
 

int     GetCpuUsage();
int     GetMemUsage();
int     GetThreadCount();

uint    StringHash(const char *str);
 

class ISerializable {
public:
    virtual ~ISerializable() {}
    virtual void    Serialize(Json::Value &root) const = 0;
    virtual void    Deserialize(Json::Value &root) = 0;
};

class Serializer {
public:
    static bool     Serialize(ISerializable *obj, std::string &output);
    static bool     Deserialzie(ISerializable *obj, std::string &input);
private:
    Serializer();
};

#endif // __ARIETIS_UTILITIES_H__