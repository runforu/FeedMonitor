#include <process.h>
#include <stdio.h>
#include "Factory.h"
#include "Loger.h"
#include "Processor.h"

void Processor::Shutdown(void) {}

inline int Processor::GetInterruptSetting(char* symbol) {
    for (int i = 0; i < m_symbol_settings_count; i++) {
        if (strcmp(symbol, m_symbol_setting[i].m_symbol) == 0) {
            LOG("Find %s interrupt time = %d", symbol, m_symbol_setting[i].m_max_interrupt_time);
            return m_symbol_setting[i].m_max_interrupt_time;
        }
    }
    return m_default_interrupt_time;
}

Processor::Processor()
    : m_reinitialize_flag(0),
      m_feed_monitor_login(31415),
      m_rejected_requests(0),
      m_requests_total(0),
      m_symbol_settings_count(0),
      m_disable_feed_monitor(0) {
    ZeroMemory(&m_manager, sizeof(m_manager));
    m_manager.login = 271828;
    COPY_STR(m_manager.name, "Feed Monitor");
    COPY_STR(m_manager.ip, "FeedMonitor");
}

void Processor::ShowStatus() {
    if (Factory::GetServerInterface() != NULL && m_requests_total > 0) {
        LOG("'%d': %d of %d requests rejected (%.2lf%%)", m_manager.login, m_rejected_requests, m_requests_total,
            m_rejected_requests * 100.0 / m_requests_total);
    }
}

void Processor::Initialize() {
    LOG("Processor-------------->Initializes");
    Factory::GetConfig()->GetInteger("Feed Monitor ID", &m_feed_monitor_login, "271828");
    m_manager.login = m_feed_monitor_login;
    Factory::GetConfig()->GetInteger("Disable Plugin", &m_disable_feed_monitor, "0");
    Factory::GetConfig()->GetInteger("default interrupt time", &m_default_interrupt_time, "60");

    ConSymbol security;
    m_symbol_settings_count = 0;
    for (int index = 0; Factory::GetServerInterface()->SymbolsNext(index, &security) != FALSE; index++) {
        LOG(security.symbol);
        if (Factory::GetConfig()->HasKey(security.symbol)) {
            LOG("Has settings for %s", security.symbol);
            if (m_symbol_settings_count >= MAX_SYMBOL_SIZE) {
                return;
            }
            COPY_STR(m_symbol_setting[m_symbol_settings_count].m_symbol, security.symbol);
            Factory::GetConfig()->GetInteger(security.symbol, &m_symbol_setting[m_symbol_settings_count++].m_max_interrupt_time,
                                             "60");
        }
    }

#ifdef _RELEASE_LOG_
    for (int i = 0; i < m_symbol_settings_count; i++) {
        LOG("%s max interrupt time: %d", m_symbol_setting[i].m_symbol, m_symbol_setting[i].m_max_interrupt_time);
    }
#endif  // _RELEASE_LOG_
}

int Processor::FilterTradeRequest(RequestInfo* request) {
    LOG("Processor::FilterTradeRequest in thread = %d", GetCurrentThreadId());
    if (m_disable_feed_monitor) {
        return RET_OK;
    }

    //--- reinitialize if configuration changed
    if (InterlockedExchange(&m_reinitialize_flag, 0) != 0) {
        Initialize();
    }
    m_requests_total++;

    int diff = GetInterruptSetting(request->trade.symbol);
    LOG("setting for %s max interrupt time = %d, current time = %d ", request->trade.symbol, diff,
        Factory::GetServerInterface()->TradeTime());

    if (diff != -1 && m_tick_map.BeforeTime(request->trade.symbol, Factory::GetServerInterface()->TradeTime() - diff)) {
        LOG("FilterTradeRequest --------> RET_TRADE_OFFQUOTES");
        m_rejected_requests++;
        return RET_TRADE_OFFQUOTES;
    }
    LOG("FilterTradeRequest --------> RET_OK");

    return RET_OK;
}

void Processor::TickApply(const ConSymbol* symbol, FeedTick* tick) {
    // Lock();
    LOG("Processor::TickApply in thread = %d", GetCurrentThreadId());
    m_tick_map.AddTick(symbol, tick);
    // Unlock();
}
