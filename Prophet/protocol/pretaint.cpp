#include "stdafx.h"
#include "pretaint.h"
#include "event.h"

PreTainter::PreTainter( Protocol *protocol )
    : m_protocol(protocol)
{

}

PreTainter::~PreTainter()
{

}

void PreTainter::OnWinapiPostCall( WinapiPostCallEvent &event )
{
    //LxWarning("Winapi post call: %d\n", event.ApiIndex);
}
