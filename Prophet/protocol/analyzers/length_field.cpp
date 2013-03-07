#include "stdafx.h"
#include "protocol/analyzer.h"

class LengthFieldDetector : public ProtocolAnalyzer {
public:
    LengthFieldDetector() : ProtocolAnalyzer(1000, "LengthFieldDetector", 0)
    {

    }


};

LengthFieldDetector __haha__whatever_this_is__;