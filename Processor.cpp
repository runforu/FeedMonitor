#include <process.h>
#include <stdio.h>
#include "ServerApi.h"
#include "Loger.h"
#include "Processor.h"

void Processor::Shutdown(void) {
    ShowStatus();
}

inline int Processor::GetInterruptSetting(char* symbol) {
    for (int i = 0; i < m_symbol_settings_count; i++) {
        if (strcmp(symbol, m_symbol_setting[i].m_symbol) == 0) {
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

Processor& Processor::Instance() {
    static Processor _instance;
    return _instance;
}

void Processor::ShowStatus() {
    if (ServerApi::Api() != NULL && m_requests_total > 0) {
        LOG("'%d': %d of %d requests rejected (%.2lf%%)", m_manager.login, m_rejected_requests, m_requests_total,
            m_rejected_requests * 100.0 / m_requests_total);
    }
}

void Processor::Initialize() {
    FUNC_WARDER;

    Config::Instance().GetInteger("Feed Monitor ID", &m_feed_monitor_login, "271828");
    m_manager.login = m_feed_monitor_login;
    Config::Instance().GetInteger("Disable Plugin", &m_disable_feed_monitor, "0");
    Config::Instance().GetInteger("default interrupt time", &m_default_interrupt_time, "60");

    ConSymbol security;
    m_symbol_settings_count = 0;
    for (int index = 0; ServerApi::Api()->SymbolsNext(index, &security) != FALSE; index++) {
        LOG(security.symbol);
        if (Config::Instance().HasKey(security.symbol)) {
            if (m_symbol_settings_count >= MAX_SYMBOL_SIZE) {
                return;
            }
            COPY_STR(m_symbol_setting[m_symbol_settings_count].m_symbol, security.symbol);
            int value = 0;
            Config::Instance().GetInteger(security.symbol, &value, "60");
            m_symbol_setting[m_symbol_settings_count++].m_max_interrupt_time = value;
            LOG("%s max interrupt time: %d", security.symbol, value);
        }
    }
}

int Processor::FilterTradeRequest(TradeTransInfo* trans) {
    FUNC_WARDER;

    //--- reinitialize if configuration changed
    if (InterlockedExchange(&m_reinitialize_flag, 0) != 0) {
        Initialize();
    }

    if (m_disable_feed_monitor) {
        return RET_OK;
    }

    m_requests_total++;

    int diff = GetInterruptSetting(trans->symbol);
    LOG("setting for %s max interrupt time = %d, current time = %d ", trans->symbol, diff,
        ServerApi::Api()->TradeTime());

    if (diff != -1 && m_tick_map.BeforeTime(trans->symbol, ServerApi::Api()->TradeTime() - diff)) {
        LOG("Quote interrupted for a long time.");
        m_rejected_requests++;
        return RET_TRADE_OFFQUOTES;
    }

    return RET_OK;
}

void Processor::TickApply(const ConSymbol* symbol, FeedTick* tick) {
    m_tick_map.AddTick(symbol, tick);
}
