#include "stdafx.h"
#include "cryptohelp.h"
#include "utilities.h"

#include "zlib/zlib.h"

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

void Checksums::GetChecksumType( u8 val8, u32 val32, std::string &s, int &len )
{
    if (val8 == Sum8) {
        s = "Sum 8-bit"; len = 1;
    } else if (val8 == Xor) {
        s = "Xor 8-bit"; len = 1;
    } else if (val8 == And) {
        s = "And 8-bit"; len = 1;
    } else if (val8 == Or) {
        s = "Or 8-bit"; len = 1;
    } else if (val32 == Sum32) { 
        s = "Sum 32-bit"; len = 4;
    } else if (val32 == Crc32) {
        s = "CRC 32-bit"; len = 4;
    } else if (val32 == Adler32) {
        s = "Adler 32-bit"; len = 4;
    } else {
        s = "Generic"; len = 4;
    }
}

Checksums::Checksums( cpbyte data, int len )
{
    Sum8 = Xor = And = Or = 0;
    Sum32 = Crc32 = Adler32 = 0;
    for (int i = 0; i < len; i++) {
        Sum8 += data[i];
        Xor ^= data[i];
        And &= data[i];
        Or |= data[i];
        Sum32 += data[i];
    }

    Adler32 = adler32(0L, Z_NULL, 0);
    Adler32 = adler32(Adler32, data, len);
    Crc32 = crc32(0L, Z_NULL, 0);
    Crc32 = crc32(Crc32, data, len);
}

EntropyMetrics::EntropyMetrics( cpbyte pin, int lin, cpbyte pout, int lout )
{
    Hi = CalculateEntropy(pin, lin);
    Ho = CalculateEntropy(pout, lout);
    Li = lin;
    Lo = lout;
    HoDivHi = Ho / Hi;
    if (HoDivHi >= 1.1) {
        Result = Li > Lo ? "Hash" : "Encryption";
    } else if (HoDivHi >= 0.9) {
        Result = Li > Lo ? "Decoding" : "Encoding";
    } else {
        Result = Li == Lo ? "Decryption" : "Unknown";
    }
}
