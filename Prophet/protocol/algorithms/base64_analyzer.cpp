#include "stdafx.h"
#include "base64_analyzer.h"

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'};
static char decoding_table[256];
static int mod_table[] = {0, 2, 1};


void base64_encode(const unsigned char *data, int input_length, byte *output_buf, int *output_length) 
{

    *output_length = 4 * ((input_length + 2) / 3);

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output_buf[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        output_buf[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        output_buf[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        output_buf[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        output_buf[*output_length - 1 - i] = '=';
}


void base64_decode(const char *data, int input_length, byte *buf, int *output_length) 
{
    if (input_length % 4 != 0) return;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
            + (sextet_b << 2 * 6)
            + (sextet_c << 1 * 6)
            + (sextet_d << 0 * 6);

        if (j < *output_length) buf[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) buf[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) buf[j++] = (triple >> 0 * 8) & 0xFF;
    }
}


void build_decoding_table() 
{
    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

bool Base64Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), true);
#endif

    for (auto &input : ctx.InputRegions) {
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        u32 olen = trs[0].Len / 4 * 3;
        for (auto &output : ctx.OutputRegions) {
            if (output.Len <= olen - 3 || output.Len > olen)
                continue;

            if (TestBase64(ctx, input, output, trs[0]))
                return true;
        }
    }

    return false;
}

bool Base64Analyzer::TestBase64(const ProcContext &ctx, const MemRegion &input, 
                                const MemRegion &output, const TaintRegion &tr )
{
    Taint tfirst = ctx.Outputs.find(output.Addr)->second.Tnt;
    auto trs1 = tfirst.GenerateRegions();
    if (trs1.size() != 1) return false;
    int ostart = trs1[0].Offset;
    for (u32 addr = output.Addr; addr < output.Addr + output.Len; addr++) {
        Taint t = ctx.Outputs.find(addr)->second.Tnt;
        auto tr = t.GenerateRegions();
        if (tr.size() != 1) return false;
        if (tr[0].Offset != ostart + (addr - output.Addr) / 3 * 4 || tr[0].Len > 4)
            return false;
    }

    pbyte pin = new byte[input.Len];
    pbyte pout = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);

    pbyte pactual = new byte[input.Len];
    int lactual;
    base64_encode(pout, output.Len, pactual, &lactual);

    bool found = false;

    if (lactual != input.Len) {
        LxDebug("base64 length mismatch\n");
        goto L_END;
    }

    
    if (CompareByteArray(pactual, pin, input.Len) == 0) {
        AlgTag *tag = new AlgTag("Base64", "Decoding", ctx.Proc->Entry());
        tag->AddParam("Encoded string", input, pin);
        tag->AddParam("Decoded string", output, pout);
        LxInfo("Base64 decoding: %08x-%08x\n", output.Addr, output.Addr + output.Len);
        m_algEngine->EnqueueNewMessage(output, pout, tr, tag, ctx, true);
        found = true;
    }

L_END:
    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);
    SAFE_DELETE_ARRAY(pactual);

    return found;
}

Base64Analyzer::Base64Analyzer()
{
    build_decoding_table();
}
