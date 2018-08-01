#ifndef _TICKMAP_H_
#define _TICKMAP_H_
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "../include/MT4ServerAPI.h"
#include "common.h"

struct SymbolTick {
    char m_symbol[12];
    TickAPI m_latest_tick;
};

class TickMap {
public:
    void AddTick(const ConSymbol* symbol, FeedTick* tick);
    void DumpTickPool(const char* symbol);
    bool BeforeTime(const char* symbol, time_t t);
    TickAPI FindTick(const char* symbol);
    TickMap() : m_symbol_count(0) { ZeroMemory(m_tick_pool, sizeof(m_tick_pool)); }

private:
    int FindTickIndex(const char* symbol);
    SymbolTick m_tick_pool[MAX_SYMBOL_SIZE];
    int m_symbol_count;
};

#endif  // !_TICKHISTORY_H_
