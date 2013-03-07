#include "stdafx.h"
#include "controller.h"
#include "protocol.h"

Controller::Controller( Protocol *protocol )
    : m_protocol(protocol)
{
    
}

Controller::~Controller()
{

}

void Controller::Initialize()
{
    m_protocol->SetInSession(); // todo
}
