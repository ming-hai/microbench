﻿// -----------------------------------------------------------------------------------------
// microbench by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2017 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

#include "osdep.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <intrin.h>
#include <algorithm>
#include <chrono>
#include "cpu_info.h"
#include "simd_util.h"
#include "microbench_version.h"

#define TEST_COUNT 20000
#define BLOCK_SIZE 96

#define PROTOTYPE_NO_256(x) \
    extern int64_t runt_ ## x(uint32_t count); \
    extern int64_t runl_ ## x(uint32_t count); \
    extern int64_t runpt_ ## x(uint32_t count); \
    extern int64_t runpl_ ## x(uint32_t count);

#define PROTOTYPE(x) \
    extern int64_t runt_ ## x(uint32_t count); \
    extern int64_t runl_ ## x(uint32_t count); \
    extern int64_t runpt_ ## x(uint32_t count); \
    extern int64_t runpl_ ## x(uint32_t count); \
    extern int64_t runpt_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runpl_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_VEX(x) \
    extern int64_t runpt_ ## x ## _vex(uint32_t count); \
    extern int64_t runpl_ ## x ## _vex(uint32_t count); \
    extern int64_t runpt_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runpl_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_256(x) \
    extern int64_t runpt_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runpl_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_PAIR(x) \
    extern int64_t runtp_ ## x(uint32_t count); \
    extern int64_t runlp_ ## x(uint32_t count); \
    extern int64_t runptp_ ## x(uint32_t count); \
    extern int64_t runplp_ ## x(uint32_t count); \
    extern int64_t runptp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runplp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runts_ ## x(uint32_t count); \
    extern int64_t runls_ ## x(uint32_t count); \
    extern int64_t runpts_ ## x(uint32_t count); \
    extern int64_t runpls_ ## x(uint32_t count); \
    extern int64_t runpts_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runpls_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_PAIR_VEX(x) \
    extern int64_t runptp_ ## x ## _vex(uint32_t count); \
    extern int64_t runplp_ ## x ## _vex(uint32_t count); \
    extern int64_t runptp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runplp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runpts_ ## x ## _vex(uint32_t count); \
    extern int64_t runpls_ ## x ## _vex(uint32_t count); \
    extern int64_t runpts_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runpls_ ## x ## _vex_256(uint32_t count);

#ifdef __cplusplus
extern "C" {
#endif
    PROTOTYPE(por);
    PROTOTYPE(pand);
    PROTOTYPE(pandn);
    PROTOTYPE(pxor);
    PROTOTYPE(psllw);
    PROTOTYPE(pslldq);
    PROTOTYPE(paddw);
    PROTOTYPE(paddsw);
    PROTOTYPE(paddd);
    PROTOTYPE(paddq);
    PROTOTYPE(pmullw);
    PROTOTYPE(pmuludq);
    PROTOTYPE(pmulld);
    PROTOTYPE(pmaddwd);
    PROTOTYPE(pmaddubsw);
    PROTOTYPE(pabsw);
    PROTOTYPE(pmaxsw);
    PROTOTYPE(pminsw);
    PROTOTYPE(pavgw);
    PROTOTYPE(psadbw);
    PROTOTYPE(packssdw);
    PROTOTYPE(pmovsxwd);
    PROTOTYPE(packsswb);
    PROTOTYPE(pmovsxbw);
    PROTOTYPE(pmovsxbd);
    PROTOTYPE(punpckhwd);
    PROTOTYPE(palignr);
    PROTOTYPE(pshufb);
    PROTOTYPE(pblendw);
    PROTOTYPE_NO_256(pblendvb);
    PROTOTYPE_VEX(pblendvb);
    PROTOTYPE_VEX(pblendd);
    PROTOTYPE_256(insertf128);
    PROTOTYPE_256(permd);
    PROTOTYPE_256(permq);
    PROTOTYPE_256(perm2f128);
    PROTOTYPE_VEX(pbroadcastw);
    PROTOTYPE(addps);
    PROTOTYPE(addpd);
    PROTOTYPE(mulps);
    PROTOTYPE(mulpd);
    PROTOTYPE(divps);
    PROTOTYPE(divpd);
    PROTOTYPE(sqrtps);
    PROTOTYPE(sqrtpd);
    PROTOTYPE(rcpps);
    PROTOTYPE(rsqrtps);
    PROTOTYPE_VEX(fmadd132ps);
    PROTOTYPE_VEX(fmadd132pd);
    PROTOTYPE(maxps);
    PROTOTYPE(maxpd);
    PROTOTYPE(minps);
    PROTOTYPE(minpd);
    PROTOTYPE_PAIR(addps_mulps);
    PROTOTYPE_PAIR(addpd_mulpd);
    PROTOTYPE_PAIR_VEX(fmadd132ps_addps);
    PROTOTYPE_PAIR_VEX(fmadd132pd_addpd);
    PROTOTYPE_PAIR_VEX(fmadd132ps_mulps);
    PROTOTYPE_PAIR_VEX(fmadd132pd_mulpd);

    alignas(32) float  INIT_DIVIDEND_F[8];
    alignas(32) double INIT_DIVIDEND_D[4];
    alignas(32) float  INIT_DIVIDER_F[8];
    alignas(32) double INIT_DIVIDER_D[4];
#ifdef __cplusplus
}
#endif

typedef int64_t(*func_check)(uint32_t count);

typedef struct {
    const char *name;
    func_check latency;
    func_check throughput;
    int flops_per_128op;
    uint32_t simd;
} check_inst_t;

#define CREATE_LIST_NO256(simd_128, x, flops_128op) \
    { #x,       runl_ ## x, runt_ ## x, flops_128op, simd_128 }, \
    { #x,       runpl_ ## x, runpt_ ## x, flops_128op, simd_128 | RDTSCP }

#define CREATE_LIST(simd_128, simd_256, x, flops_128op) \
    { #x,       runl_ ## x, runt_ ## x, flops_128op, simd_128 }, \
    { #x,       runpl_ ## x, runpt_ ## x, flops_128op, simd_128 | RDTSCP }, \
    { #x "(256)", runpl_ ## x ## _vex_256, runpt_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }

#define CREATE_LIST_VEX(simd_128, simd_256, x, flops_128op) \
    { #x "(vex)", runpl_ ## x ## _vex, runpt_ ## x ## _vex, flops_128op, simd_128 | RDTSCP }, \
    { #x "(256)", runpl_ ## x ## _vex_256, runpt_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }

#define CREATE_LIST_256(simd_128, simd_256, x, flops_128op) \
    { #x "(256)", runpl_ ## x ## _vex_256, runpt_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }

#define CREATE_LIST_PAIR(simd_128, simd_256, x, flops_128op) \
    { #x "",       runlp_ ## x, runtp_ ## x, flops_128op, simd_128 }, \
    { #x "",       runplp_ ## x, runptp_ ## x, flops_128op, simd_128 | RDTSCP }, \
    { #x "(256)", runplp_ ## x ## _vex_256, runptp_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }/*, \
    { #x "serial",       runpls_ ## x, runpts_ ## x, flops_128op, simd_128 }, \
    { #x "serial(256)", runpls_ ## x ## _vex_256, runpts_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }*/

#define CREATE_LIST_PAIR_VEX(simd_128, simd_256, x, flops_128op) \
    { #x "(128)", runplp_ ## x ## _vex, runptp_ ## x ## _vex, flops_128op, simd_128 | RDTSCP }, \
    { #x "(256)", runplp_ ## x ## _vex_256, runptp_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }/*, \
    { #x "serial",       runpls_ ## x ## _vex, runpts_ ## x ## _vex, flops_128op, simd_128 | RDTSCP }, \
    { #x "serial(256)", runpls_ ## x ## _vex_256, runpts_ ## x ## _vex_256, flops_128op * 2, simd_256 | RDTSCP }*/

check_inst_t check_list[] = {
    CREATE_LIST(SSE2,  AVX2, por,    0),
    CREATE_LIST(SSE2,  AVX2, pand,   0),
    CREATE_LIST(SSE2,  AVX2, pandn,  0),
    CREATE_LIST(SSE2,  AVX2, pxor,   0),
    CREATE_LIST(SSE2,  AVX2, psllw,  0),
    CREATE_LIST(SSE2,  AVX2, pslldq, 0),
    CREATE_LIST(SSE2,  AVX2, paddw,  0),
    CREATE_LIST(SSE2,  AVX2, paddsw, 0),
    CREATE_LIST(SSE2,  AVX2, paddd,  0),
    CREATE_LIST(SSE2,  AVX2, paddq,  0),
    CREATE_LIST(SSSE3, AVX2, pabsw,  0),
    CREATE_LIST(SSE2,  AVX2, pmullw, 0),
    CREATE_LIST(SSE2,  AVX2, pmuludq, 0),
    CREATE_LIST(SSE41, AVX2, pmulld, 0),
    CREATE_LIST(SSE2,  AVX2, pmaddwd, 0),
    CREATE_LIST(SSSE3, AVX2, pmaddubsw, 0),
    CREATE_LIST(SSE2,  AVX2, pmaxsw, 0),
    CREATE_LIST(SSE2,  AVX2, pminsw, 0),
    CREATE_LIST(SSE2,  AVX2, pavgw,  0),
    CREATE_LIST(SSE2,  AVX2, psadbw, 0),
    CREATE_LIST(SSE2,  AVX2, packssdw,  0),
    CREATE_LIST(SSE41, AVX2, pmovsxwd,  0),
    CREATE_LIST(SSE2,  AVX2, packsswb,  0),
    CREATE_LIST(SSE41, AVX2, pmovsxbw,  0),
    CREATE_LIST(SSE41, AVX2, pmovsxbd,  0),
    CREATE_LIST(SSE2,  AVX2, punpckhwd,  0),
    CREATE_LIST(SSSE3, AVX2, palignr,  0),
    CREATE_LIST(SSSE3, AVX2, pshufb,  0),
    CREATE_LIST(SSE41, AVX2, pblendw, 0),
    CREATE_LIST_NO256(SSSE3, pblendvb, 0),
    CREATE_LIST_VEX(AVX, AVX2, pblendvb, 0),
    CREATE_LIST_VEX(AVX2, AVX2, pblendd, 0),
    CREATE_LIST_256(AVX, AVX2, insertf128, 0),
    CREATE_LIST_256(AVX, AVX2, perm2f128, 0),
    CREATE_LIST_256(AVX2, AVX2, permd, 0),
    CREATE_LIST_256(AVX2, AVX2, permq, 0),
    CREATE_LIST_VEX(AVX2, AVX2, pbroadcastw, 0),
    CREATE_LIST(SSE2, AVX, addps,  4),
    CREATE_LIST(SSE2, AVX, addpd,  2),
    CREATE_LIST(SSE2, AVX, mulps,  4),
    CREATE_LIST(SSE2, AVX, mulpd,  2),
    CREATE_LIST_PAIR(SSE2, AVX, addps_mulps, 4),
    CREATE_LIST_PAIR(SSE2, AVX, addpd_mulpd, 2),
    CREATE_LIST_VEX(AVX2, AVX2, fmadd132ps, 8),
    CREATE_LIST_VEX(AVX2, AVX2, fmadd132pd, 4),
    CREATE_LIST_PAIR_VEX(AVX2, AVX2, fmadd132ps_addps, 6),
    CREATE_LIST_PAIR_VEX(AVX2, AVX2, fmadd132pd_addpd, 3),
    CREATE_LIST_PAIR_VEX(AVX2, AVX2, fmadd132ps_mulps, 6),
    CREATE_LIST_PAIR_VEX(AVX2, AVX2, fmadd132pd_mulpd, 3),
    CREATE_LIST(SSE2, AVX, divps,  0),
    CREATE_LIST(SSE2, AVX, divpd,  0),
    CREATE_LIST(SSE2, AVX, sqrtps, 0),
    CREATE_LIST(SSE2, AVX, sqrtpd, 0),
    CREATE_LIST(SSE2, AVX, rcpps,   0),
    CREATE_LIST(SSE2, AVX, rsqrtps, 0),
    CREATE_LIST(SSE2, AVX, maxps,  0),
    CREATE_LIST(SSE2, AVX, maxpd,  0),
    CREATE_LIST(SSE2, AVX, minps,  0),
    CREATE_LIST(SSE2, AVX, minpd,  0),
};

void init_const() {
    for (int i = 0; i < _countof(INIT_DIVIDEND_F); i++) {
        INIT_DIVIDEND_F[i] = FLT_MAX;
    }
    for (int i = 0; i < _countof(INIT_DIVIDEND_D); i++) {
        INIT_DIVIDEND_D[i] = DBL_MAX;
    }
    for (int i = 0; i < _countof(INIT_DIVIDER_F); i++) {
        INIT_DIVIDER_F[i] = 1.0f + FLT_EPSILON;
    }
    for (int i = 0; i < _countof(INIT_DIVIDER_D); i++) {
        INIT_DIVIDER_D[i] = 1.0 + DBL_EPSILON;
    }
}

void warm_up() {
    for (int i = 0; i < 2000; i++) {
        runl_por(TEST_COUNT);
    }
}

double get_tick_per_clock() {
    const int loop_count = 1000;
    auto tick_min = runl_por(TEST_COUNT);
    for (int i = 0; i < loop_count; i++) {
        tick_min = std::min(tick_min, runl_por(TEST_COUNT));
    }
    return tick_min / ((double)BLOCK_SIZE * TEST_COUNT);
}

double get_tick_per_sec() {
    const int nMul = 100;
    runl_por(TEST_COUNT * nMul);
    auto start = std::chrono::high_resolution_clock::now();
    int64_t tick = runl_por(TEST_COUNT * nMul);
    auto fin = std::chrono::high_resolution_clock::now();
    double second = std::chrono::duration_cast<std::chrono::microseconds>(fin - start).count() * 1e-6;
    return tick / second;
}

int main(int argc, char **argv) {
    init_const();
    const uint32_t simd_avail = get_availableSIMD();
    char buffer[256];
    getCPUInfo(buffer);
    fprintf(stdout, "microbench " VER_STR_FILEVERSION " by rigaya\n");
    fprintf(stdout, "%s\n", buffer);

    SetThreadAffinityMask(GetCurrentThread(), 1);
    warm_up();
    const double tick_per_clock = get_tick_per_clock();
    const double tick_per_sec = get_tick_per_sec();
    fprintf(stdout, "core frequency: %d MHz\n", (int)(tick_per_sec / tick_per_clock * 1e-6 + 0.5));

    size_t max_name_len = 0;
    for (int i = 0; i < _countof(check_list); i++) {
        max_name_len = std::max(max_name_len, strlen(check_list[i].name));
    }
    char name[256] = { 0 };
    for (size_t j = 0; j < max_name_len; j++) {
        name[j] = ' ';
    }
    fprintf(stdout, "%s Throughput,  Latency\n", name);
    fprintf(stdout, "instruction%s        IPC,    Cycle\n", name + strlen("instruction"));

    const int loop_count = 25;
    for (int i = 0; i < _countof(check_list); i++) {
        if ((simd_avail & RDTSCP) == (check_list[i].simd & RDTSCP)
            && (simd_avail & check_list[i].simd) == check_list[i].simd) {
            auto tick_t = INT64_MAX;
            auto tick_l = INT64_MAX;
            for (int j = 0; j < loop_count; j++) {
                tick_t = std::min(tick_t, check_list[i].throughput(TEST_COUNT));
                tick_l = std::min(tick_l, check_list[i].latency(TEST_COUNT));
            }
            strcpy(name, check_list[i].name);
            for (size_t j = strlen(name); j < max_name_len; j++) {
                name[j] = ' ';
            }
            fprintf(stdout, "%s:  T: %5.2f, L: %5.2f", name,
                TEST_COUNT * (double)BLOCK_SIZE * tick_per_clock / (double)tick_t,
                1.0 / (TEST_COUNT * (double)BLOCK_SIZE * tick_per_clock / (double)tick_l));
            if (check_list[i].flops_per_128op > 0) {
                double flops = TEST_COUNT * (double)BLOCK_SIZE * check_list[i].flops_per_128op / (tick_t / tick_per_sec);
                fprintf(stdout, ",  %5.1f GFLOPS", flops * 1e-9);
            }
            fprintf(stdout, "\n");
        }
    }
}
