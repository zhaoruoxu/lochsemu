#include "stdafx.h"
#include "statistics.h"
#include "instruction.h"

LochsStatistics LxStatistics;

int cmp(const std::pair<std::string, int>& x, const std::pair<std::string, int>& y);

LochsStatistics::LochsStatistics()
{

}

LochsStatistics::~LochsStatistics()
{

}


void LochsStatistics::Initialize( void )
{
    m_enabled = g_config.GetInt("Statistics", "Enabled", 1) != 0;
}


void LochsStatistics::OnProcessPreRun( const Process *proc, Processor *cpu )
{
    if (!m_enabled) return;
    m_instCount.clear();
}

void LochsStatistics::OnProcessPostRun( const Process *proc )
{
    // output result
    if (!m_enabled) return;

    std::string filePath = LxGetModuleDirectory(g_module) + "statistics.txt";
    FILE *fp = fopen(filePath.c_str(), "w");
    if (!fp) {
        LxWarning("LochsStatistics: cannot create output file!\n");
        return;
    }

    std::vector<std::pair<std::string, int> > v;
    std::map<std::string, int>::iterator iter = m_instCount.begin();
    for (; iter != m_instCount.end(); iter++) {
        v.push_back(std::make_pair(iter->first, iter->second));
    }
    std::sort(v.begin(), v.end(), cmp);

    fprintf(fp, "Instruction count statistics\n");
    i64 total = 0;
    for (uint i = 0; i < v.size(); i++) {
        fprintf(fp, "%-10s%d\n", v[i].first.c_str(), v[i].second);
        total += v[i].second;
    }
    fprintf(fp, "total:    %I64d\n", total);

    fclose(fp);
}

void LochsStatistics::OnProcessorPreExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;

    std::string mnemonic(inst->Main.Inst.Mnemonic);
    std::map<std::string, int>::iterator iter = m_instCount.find(mnemonic);
    if (iter == m_instCount.end()) {
        m_instCount[mnemonic] = 1;
    } else {
        iter->second++;
    }
}


int cmp(const std::pair<std::string, int>& x, const std::pair<std::string, int>& y)  
{  
    return x.second > y.second;  
}  