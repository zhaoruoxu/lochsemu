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

void ChainedXor_Decrypt( cpbyte ct, pbyte pt, int len )
{
    pt[0] = ct[0];
    for (int i = len - 1; i > 0; i--)
        pt[i] = ct[i] ^ ct[i-1];
}

bool ChainedXor_IsValidDecrypt( cpbyte ct, cpbyte pt, int ctlen )
{
    pbyte t = new byte[ctlen];
    ChainedXor_Decrypt(ct, t, ctlen);
    bool r = CompareByteArray(pt, t, ctlen) == 0;
    SAFE_DELETE_ARRAY(t);
    return r;
}

double CalculateEntropy( cpbyte data, int len )
{
    if (len <= 1) return 0;
    double count[256];
    for (int i = 0; i < 256; i++)
        count[i] = 0;
    for (int i = 0; i < len; i++)
        count[data[i]] += 1.0 / len;
    double entropy = 0;
    for (int i = 0; i < 256; i++)
        if (count[i] > 0)
            entropy += count[i] * log(count[i]);
    return -entropy / log(len);
}
