#pragma once
 
#ifndef __ARIETIS_GUI_GUI_H__
#define __ARIETIS_GUI_GUI_H__
 
#include "Arietis.h"

void RunGUI();

class ArietisFrame;
class TaintPanel;

class ArietisApp : public wxApp {
public:
    virtual bool        OnInit();
};

#endif // __ARIETIS_GUI_GUI_H__