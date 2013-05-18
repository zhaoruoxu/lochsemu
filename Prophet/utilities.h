#pragma once
 
#ifndef __PROPHET_UTILITIES_H__
#define __PROPHET_UTILITIES_H__
 

int     GetCpuUsage();
int     GetMemUsage();
int     GetThreadCount();

uint    StringHash(const char *str);
uint    FileTimeHash(const char *path);
 

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

class File {
public:
    File(const char *filename, const char *mode);
    File(const std::string &filename, const std::string &mode);
    ~File();

    FILE *  Ptr() { return m_fp; }
    const FILE *Ptr() const { return m_fp; }
    void    Close();

private:
    void    Open(const char *filename, const char *mode);
    FILE *m_fp;
};

#endif // __PROPHET_UTILITIES_H__