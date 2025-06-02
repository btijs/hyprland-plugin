#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/managers/EventManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include "SyncedMonitors.hpp"

inline HANDLE PHANDLE = nullptr;

static CSharedPointer<HOOK_CALLBACK_FN> onWorkSpaceChangeHook = nullptr;
static CSharedPointer<HOOK_CALLBACK_FN> onMonitorRemovedHook = nullptr;
static CSharedPointer<HOOK_CALLBACK_FN> onMonitorAddedHook = nullptr;

bool monitorLayoutChanging = false;

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

SDispatchResult onWorkSpaceChange(void *data, SCallbackInfo &info, std::any val) {
    if (monitorLayoutChanging) {
        Debug::log(INFO, "Workspace change ignored due to monitor layout change.");
        return SDispatchResult{};
    }
    const auto focused_workspace = std::any_cast<PHLWORKSPACE>(val);
    SyncedMonitors::changeWorkspaces(SyncedMonitors::translateWorkspaceToDesk(focused_workspace->m_id));
    return SDispatchResult{};
}

SDispatchResult focusWorkSpace(std::string arg) {
    SyncedMonitors::changeWorkspaces(stoi(arg));
    return SDispatchResult{};
}

SDispatchResult nextWorkSpace(std::string arg) {
    SyncedMonitors::nextWorkspaces();
    return SDispatchResult{};
}

SDispatchResult previousWorkSpace(std::string arg) {
    SyncedMonitors::previousWorkspaces();
    return SDispatchResult{};
}

SDispatchResult moveToWorkSpace(std::string arg) {
    SyncedMonitors::moveToWorkspace(stoi(arg));
    return SDispatchResult{};
}

SDispatchResult moveToNextWorkspace(std::string arg) {
    SyncedMonitors::moveToNextWorkspace();
    return SDispatchResult{};
}

SDispatchResult moveToPreviousWorkspace(std::string arg) {
    SyncedMonitors::moveToPreviousWorkspace();
    return SDispatchResult{};
}

void onMonitorPreRemoved(void *data, SCallbackInfo &info, std::any val) {
    monitorLayoutChanging = true;
}

void onMonitorRemoved(void *data, SCallbackInfo &info, std::any val) {
    if (g_pCompositor->m_monitors.size() == 0) {
        return;
    }
        monitorLayoutChanging = false;
        // Rename the moved workspaces to match the new monitor
        // Signal IPC that a monitor was removed
        for (const auto &workspace : g_pCompositor->m_workspaces) {
            SyncedMonitors::fixWorkSpaceName(workspace);
            g_pEventManager->postEvent(SHyprIPCEvent{"workspace", workspace->m_name});
        }
}

void onMonitorPreAdded(void *data, SCallbackInfo &info, std::any val) {
    monitorLayoutChanging = true;
}

void onMonitorAdded(void *data, SCallbackInfo &info, std::any val) {
    monitorLayoutChanging = false;
    // Rename the moved workspace to match the new monitor
    // And signal IPC that a monitor was removed
    for (const auto &workspace : g_pCompositor->m_workspaces) {
        SyncedMonitors::fixWorkSpaceName(workspace);
        g_pEventManager->postEvent(SHyprIPCEvent{"workspace", workspace->m_name});
    }
}

// This function is called when the plugin is loaded.
APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[CUSTOM-PLUGIN] Mismatched headers! Can't proceed.",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch" + HASH + " != " + GIT_COMMIT_HASH);
    }

    SyncedMonitors::initializeWorkspaces();

    HyprlandAPI::addDispatcherV2(PHANDLE, "syncworkspace", focusWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "nextsyncedworkspace", nextWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "previoussyncedworkspace", previousWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "movetosyncedworkspace", moveToWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "movetonextsyncedworkspace", moveToNextWorkspace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "movetoprevioussyncedworkspace", moveToPreviousWorkspace);

    onMonitorRemovedHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "monitorRemoved", onMonitorRemoved);
    onMonitorAddedHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "monitorAdded", onMonitorAdded);

    onWorkSpaceChangeHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "workspace", onWorkSpaceChange);

    return {"Custom Plugin", "A plugin to keep the workspace on both montitors synces", "Tijs", "1.0"};
}
