#include "Config.h"
#include "Factory.h"
#include "Loger.h"
#include "Processor.h"

PluginInfo ExtPluginInfo = {"Feed Monitor", 1, "DH Copyright.", {0}};

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            char tmp[256], *cp;
            //--- create configuration filename
            GetModuleFileName((HMODULE)hModule, tmp, sizeof(tmp) - 5);
            if ((cp = strrchr(tmp, '.')) != NULL) {
                *cp = 0;
                strcat(tmp, ".ini");
            }
            Factory::GetConfig()->Load(tmp);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            Factory::GetProcessor()->ShowStatus();
            break;
    }
    return (TRUE);
}

void APIENTRY MtSrvAbout(PluginInfo* info) {
    if (info != NULL) {
        memcpy(info, &ExtPluginInfo, sizeof(PluginInfo));
    }
}

int APIENTRY MtSrvStartup(CServerInterface* server) {
    if (server == NULL) {
        return (FALSE);
    }
    //--- check version
    if (server->Version() != ServerApiVersion) {
        return (FALSE);
    }
    //--- save server interface link
    Factory::SetServerInterface(server);

    //--- initialize dealer helper
    Factory::GetProcessor()->Initialize();

    return (TRUE);
}

void APIENTRY MtSrvCleanup() {
    Factory::GetProcessor()->Shutdown();
    Factory::SetServerInterface(NULL);
}

int APIENTRY MtSrvPluginCfgSet(const PluginCfg* values, const int total) {
    LOG("MtSrvPluginCfgSet total = %d.", total);
    int res = Factory::GetConfig()->Set(values, total);
    Factory::GetProcessor()->Reinitialize();
    return (res);
}

int APIENTRY MtSrvPluginCfgNext(const int index, PluginCfg* cfg) {
    LOG("MtSrvPluginCfgNext index=%d, name=%s, value=%s.", index, cfg->name, cfg->value);
    return Factory::GetConfig()->Next(index, cfg);
}

int APIENTRY MtSrvPluginCfgTotal() {
    LOG("MtSrvPluginCfgTotal.");
    return Factory::GetConfig()->Total();
}

int APIENTRY MtSrvTradeRequestFilter(RequestInfo* request, const int isdemo) {
    LOG("MtSrvTradeRequestFilter.");
    return Factory::GetProcessor()->FilterTradeRequest(request);
}

void APIENTRY MtSrvHistoryTickApply(const ConSymbol* symbol, FeedTick* inf) { Factory::GetProcessor()->TickApply(symbol, inf); }