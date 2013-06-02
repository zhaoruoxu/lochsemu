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
std::string ByteArrayToDotString(cpbyte data, int n, int maxlen);

template <typename T>
class Array {
public:
    Array(int initSize = 32) {
        m_currSize = 0;
        m_currMaxSize = initSize;
        m_data = new T[m_currMaxSize];
    }
    ~Array() {
        SAFE_DELETE_ARRAY(m_data);
    }

    T *Get() const { return m_data; }
    T Get(int n) const { Assert(n < m_currSize); return m_data[n]; }
    int Size() const { return m_currSize; }
    void Append(const T *data, int n) {
        if (m_currSize + n > m_currMaxSize)
            Expand();
        memcpy(m_data + m_currSize, data, n * sizeof(T));
        m_currSize += n;
    }

private:
    void Expand() {
        m_currMaxSize *= 2;
        T *newData = new T[m_currMaxSize];
        memcpy(newData, m_data, m_currSize * sizeof(T));
        SAFE_DELETE_ARRAY(m_data);
        m_data = newData;
    }

private:
    T *m_data;
    int m_currSize;
    int m_currMaxSize;
};

#endif // __PROPHET_UTILITIES_H__