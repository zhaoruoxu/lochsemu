#pragma once
 
#ifndef __ARIETIS_GUI_CPUPANEL_H__
#define __ARIETIS_GUI_CPUPANEL_H__
 
#include "Arietis.h"
#include "gui.h"

class CpuPanel : public wxPanel {
public:
    CpuPanel(wxWindow *parent);
private:
    void        InitUI();
};
 
#endif // __ARIETIS_GUI_CPUPANEL_H__