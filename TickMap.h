#ifndef _TICKMAP_H_
#define _TICKMAP_H_
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "Synchronizer.h"
#include "../include/MT4ServerAPI.h"

#define MAX_SYMBOL_SIZE 128

struct SymbolTick {
    char m_symbol[12];
    TickAPI m_latest_tick;
};

class TickMap {
public:
    void AddTick(const ConSymbol* symbol, FeedTick* tick);
    bool BeforeTime(const char* symbol, time_t t);
    TickMap() : m_symbol_count(0) {
        ZeroMemory(m_tick_pool, sizeof(m_tick_pool));
    }

private:
    int FindTickIndex(const char* symbol);
    SymbolTick m_tick_pool[MAX_SYMBOL_SIZE];
    int m_symbol_count;
    // Synchronizer m_synchronizer;
};

#endif  // !_TICKHISTORY_H_
