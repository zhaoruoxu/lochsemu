#include "stdafx.h"
#include "statistics.h"

Statistics::Statistics()
{
    m_data = NULL;
    m_ptr = 0;
}

Statistics::~Statistics()
{
    SAFE_DELETE_ARRAY(m_data);
}

void Statistics::Initialize()
{
    m_total = max(g_config.GetInt("Statistics", "Granularity", 200), 10);
    m_data = new uint[m_total];
    ZeroMemory(m_data, sizeof(uint) * m_total);
}

void Statistics::OnPreExecute( PreExecuteEvent &event )
{
    m_data[m_ptr]++;
}

uint Statistics::Get( int n ) const
{
    Assert(n >= 0 && n < m_total);
    int i = m_ptr - n;
    if (i < 0) i += m_total;
    return m_data[i];
}

uint Statistics::GetMax() const
{
    uint m = m_data[0];
    for (int i = 1; i < m_total; i++)
        if (m_data[i] > m) m = m_data[i];
    return m;
}

void Statistics::Advance()
{
    m_ptr++;
    if (m_ptr == m_total)
        m_ptr = 0;

    m_data[m_ptr] = 0;
}
