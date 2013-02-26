#pragma once
 
#ifndef __ARIETIS_UTILITIES_H__
#define __ARIETIS_UTILITIES_H__
 

int     GetCpuUsage();
int     GetMemUsage();
int     GetThreadCount();

uint    StringHash(const char *str);
 
#endif // __ARIETIS_UTILITIES_H__