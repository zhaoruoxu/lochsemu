#include "stdafx.h"
#include "archive.h"

Archive::Archive()
{
}

Archive::~Archive()
{

}

void Archive::Serialize( Json::Value &root ) const
{
    for (auto &obj : m_objects) {
        Json::Value objRoot;
        obj.second->Serialize(objRoot);
        root[obj.first] = objRoot;
    }
}

void Archive::Deserialize( Json::Value &root )
{
    for (auto &obj : m_objects) {
        Json::Value objRoot = root[obj.first];
        if (!objRoot.isNull())
            obj.second->Deserialize(objRoot);
    }
}

