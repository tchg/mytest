#include "GDT.h"

DWORD GetBaseAddr(PSEGMENT_DESCRIPTOR psd)
{
    BASEADDR bs;
    bs.base_high = psd->base_high;
    bs.base_mid = psd->base_mid;
    bs.base_low = psd->base_low;
    return *(DWORD*)&bs;
}

DWORD GetLimit(PSEGMENT_DESCRIPTOR psd)
{
    DWORD limit = 0;
    LIMITADDR ld = { 0 };
    ld.limit_high = psd->limit_high;
    ld.limit_low = psd->limit_low;
    limit = *(DWORD*)&ld;
    if (psd->G)
    {
        //Èç¹ûÊÇlimit * 4k +0xfff
        limit = (limit << 12) + 0xfff;
    }
    return limit;
}

DWORD GetIdtAddr(PMYIDT pidt)
{
    DWORD limit = 0;
    limit = (pidt->hei << 16) + pidt->low ;
    return limit;
}
