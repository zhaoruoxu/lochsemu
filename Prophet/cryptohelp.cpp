#include "stdafx.h"
#include "cryptohelp.h"
#include "utilities.h"

void RC4_KeySchedule( pbyte S, const pbyte key, int n )
{
    for (int i = 0; i < 256; i++)
        S[i] = i;
    byte j = 0;
    for (int i = 0; i < 256; i++) {
        j += S[i] + (byte) key[i % n];
        std::swap(S[i], S[j]);
    }
}

bool RC4_IsValidSbox( const pbyte S, const pbyte key, int keylen )
{
    byte sbox[256];
    RC4_KeySchedule(sbox, key, keylen);
    return CompareByteArray(S, sbox, 256) == 0;
}
