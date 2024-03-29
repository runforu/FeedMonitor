#include <time.h>
#include "Loger.h"
#include "TickMap.h"

void TickMap::AddTick(const ConSymbol* symbol, FeedTick* tick) {
    if (tick->ctm == 0) {
        return;
    }

    int index = FindTickIndex(symbol->symbol);

    SymbolTick* tp = NULL;
    if (index != -1) {
        tp = &m_tick_pool[index];
    } else {
        if (m_symbol_count == MAX_SYMBOL_SIZE) {
            return;
        } else {
            tp = &m_tick_pool[m_symbol_count++];
            COPY_STR(tp->m_symbol, symbol->symbol);
        }
    }

    tp->m_latest_tick.ctm = tick->ctm;
    tp->m_latest_tick.ask = tick->ask;
    tp->m_latest_tick.bid = tick->bid;

    return;
}

bool TickMap::BeforeTime(const char* symbol, time_t t) {
    int index = FindTickIndex(symbol);
    if (index == -1) {
        return true;
    }

    SymbolTick* tp = &m_tick_pool[index];
    LOG("%s --->last update time: %d", symbol, tp->m_latest_tick.ctm);
    return t >= tp->m_latest_tick.ctm;
}

int TickMap::FindTickIndex(const char* symbol) {
    int index = 0;
    for (; index < m_symbol_count; index++) {
        if (strcmp(m_tick_pool[index].m_symbol, symbol) == 0) {
            break;
        }
    }
    return index == m_symbol_count ? -1 : index;
}
