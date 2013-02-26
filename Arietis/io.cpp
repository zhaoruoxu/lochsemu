#include "stdafx.h"
#include "io.h"


bool Serializer::Serialize( ISerializable *obj, std::string &output )
{
    if (NULL == obj) return false;

    Json::Value root;
    obj->Serialize(root);

    Json::StyledWriter  writer;
    output = writer.write(root);
    return true;
}

bool Serializer::Deserialzie( ISerializable *obj, std::string &input )
{
    if (NULL == obj) return false;

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(input, root))
        return false;

    obj->Deserialize(root);
    return true;
}
