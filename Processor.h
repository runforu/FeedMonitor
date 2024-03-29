#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "Config.h"
#include "TickMap.h"
#include "common.h"

struct SymbolSetting {
    char m_symbol[12];
    int m_max_interrupt_time;
    SymbolSetting() : m_max_interrupt_time(-1) {}
};

class Processor {
private:
    //--- configurations
    int m_disable_feed_monitor;
    int m_default_interrupt_time;
    SymbolSetting m_symbol_setting[MAX_SYMBOL_SIZE];
    int m_symbol_settings_count;

    TickMap m_tick_map;
    LONG m_reinitialize_flag;

    int m_requests_total;
    int m_rejected_requests;

    //Synchronizer m_sync;

public:
    static Processor& Instance();

    inline void Reinitialize() {
        InterlockedExchange(&m_reinitialize_flag, 1);
    }
    void ShowStatus();
    int FilterTradeRequest(TradeTransInfo* request);
    void TickApply(const ConSymbol* symbol, FeedTick* tick);
    void Initialize();
    void Shutdown(void);

private:
    inline int GetInterruptSetting(char* symbol);
    Processor();
};

//+------------------------------------------------------------------+
#endif  // !_PROCESSOR_H_
