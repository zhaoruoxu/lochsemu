#pragma once
 
#ifndef __PROPHET_STATISTICS_H__
#define __PROPHET_STATISTICS_H__
 
#include "prophet.h"

class Statistics {
public:
    Statistics();
    ~Statistics();

    void        Initialize();
    void        OnPreExecute(PreExecuteEvent &event);

    void        Advance();
    uint        Get(int n) const;
    uint        GetMax() const;
    int         GetTotal() const { return m_total; }

private:
    int         m_total;
    int         m_ptr;
    uint *      m_data;
};
 
#endif // __PROPHET_STATISTICS_H__