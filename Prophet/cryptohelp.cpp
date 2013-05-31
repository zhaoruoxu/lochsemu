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

void RC4_Crypt( pbyte S, const pbyte src, pbyte dest, int n )
{
    byte a = 0, b = 0;
    for (int i = 0; i < n; i++) {
        a++;
        b += S[a];
        std::swap(S[a], S[b]);
        dest[i] = src[i] ^ S[(S[a] + S[b]) & 0xff];
    }
}

bool RC4_IsValidCrypt( pbyte S, const pbyte pt, const pbyte ct, int n )
{
    pbyte o = new byte[n];
    RC4_Crypt(S, pt, o, n);
    bool valid = CompareByteArray(o, ct, n) == 0;
    SAFE_DELETE_ARRAY(o);
    return valid;
}
