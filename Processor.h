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
    friend class Factory;

private:
    //--- dealer user info
    UserInfo m_manager;

    //--- configurations
    int m_feed_monitor_login;
    int m_disable_feed_monitor;
    int m_default_interrupt_time;
    SymbolSetting m_symbol_setting[MAX_SYMBOL_SIZE];
    int m_symbol_settings_count;

    TickMap m_tick_map;
    LONG m_reinitialize_flag;

    Synchronizer m_sync;

public:
    inline void Reinitialize() { InterlockedExchange(&m_reinitialize_flag, 1); }
    void ShowStatus();
    int FilterTradeRequest(RequestInfo* request);
    void TickApply(const ConSymbol* symbol, FeedTick* tick);
    void Initialize();
    void Shutdown(void);

private:
    inline int GetInterruptSetting(char * symbol);
    inline void Lock() { m_sync.Lock(); }
    inline void Unlock() { m_sync.Unlock(); }
    Processor();
};

//+------------------------------------------------------------------+
#endif  // !_PROCESSOR_H_
