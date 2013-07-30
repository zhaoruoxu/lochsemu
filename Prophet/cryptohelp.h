#pragma once
 
#ifndef __PROPHET_CRYPTOHELP_H__
#define __PROPHET_CRYPTOHELP_H__
 
#include "prophet.h"

void RC4_KeySchedule(pbyte S, const pbyte key, int n);
bool RC4_IsValidSbox(const pbyte S, const pbyte key, int keylen);
void RC4_Crypt(pbyte S, const pbyte src, pbyte dest, int n);
bool RC4_IsValidCrypt(pbyte S, const pbyte pt, const pbyte ct, int n);
void ChainedXor_Decrypt(cpbyte ct, pbyte pt, int len);
bool ChainedXor_IsValidDecrypt(cpbyte ct, cpbyte pt, int ctlen);

double CalculateEntropy(cpbyte data, int len);

struct Checksums {
    u32 Sum32;
    u8 Sum8;
    u8 Xor;
    u8 And;
    u8 Or;
    u32 Crc32;
    u32 Adler32;

    Checksums(cpbyte data, int len);
    void GetChecksumType(u8 val8, u32 val32, std::string &s, int &len);
};

#endif // __PROPHET_CRYPTOHELP_H__