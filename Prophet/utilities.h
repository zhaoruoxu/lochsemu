#pragma once
 
#ifndef __PROPHET_UTILITIES_H__
#define __PROPHET_UTILITIES_H__
 

int     GetCpuUsage();
int     GetMemUsage();
int     GetThreadCount();

uint    StringHash(const char *str);
uint    FileTimeHash(const char *path);
 
int     CompareByteArray(const pbyte l, const pbyte r, int n);

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
    virtual ~File();

    virtual FILE *  Ptr() { return m_fp; }
    virtual const FILE *Ptr() const { return m_fp; }
    void    Close();

private:
    void    Open(const char *filename, const char *mode);
    FILE *  m_fp;
};

class StdOut : public File {
public:
    StdOut() : File(NULL, NULL) {}
    virtual FILE *  Ptr() { return stdout; }
    virtual const FILE *Ptr() const { return stdout; }
};


void DotToImage(const std::string &filename);

#endif // __PROPHET_UTILITIES_H__