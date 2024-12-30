#pragma once

#include "pe_crt/crt_types.hpp"

#include "nt_headers.hpp"

namespace portable_executable
{
    static constexpr pe_crt::uint16_t dos_magic = 0x5A4D;

    struct dos_header_t
    {
        pe_crt::uint16_t e_magic;
        pe_crt::uint16_t e_cblp;
        pe_crt::uint16_t e_cp;
        pe_crt::uint16_t e_crlc;
        pe_crt::uint16_t e_cparhdr;
        pe_crt::uint16_t e_minalloc;
        pe_crt::uint16_t e_maxalloc;
        pe_crt::uint16_t e_ss;
        pe_crt::uint16_t e_sp;
        pe_crt::uint16_t e_csum;
        pe_crt::uint16_t e_ip;
        pe_crt::uint16_t e_cs;
        pe_crt::uint16_t e_lfarlc;
        pe_crt::uint16_t e_ovno;
        pe_crt::uint16_t e_res[4];
        pe_crt::uint16_t e_oemid;
        pe_crt::uint16_t e_oeminfo;
        pe_crt::uint16_t e_res2[10];
        pe_crt::uint32_t e_lfanew;

        bool valid() const;

        nt_headers_t* nt_headers();

        const nt_headers_t* nt_headers() const;
    };
}