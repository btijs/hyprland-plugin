#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/desktop/Workspace.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include "SyncedMonitors.hpp"

inline HANDLE PHANDLE = nullptr;

static CSharedPointer<HOOK_CALLBACK_FN> onWorkSpaceChangeHook = nullptr;

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

SDispatchResult onWorkSpaceChange(void *data, SCallbackInfo &info, std::any val) {
    const auto focused_workspace = std::any_cast<PHLWORKSPACE>(val);
    SyncedMonitors::changeWorkspaces(SyncedMonitors::translateWorkspaceToDesk(focused_workspace->m_iID));
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

// This function is called when the plugin is loaded.
APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[CUSTOM-PLUGIN] Mismatched headers! Can't proceed.",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch");
    }

    SyncedMonitors::initializeWorkspaces();

    HyprlandAPI::addDispatcherV2(PHANDLE, "syncworkspace", focusWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "nextsyncedworkspace", nextWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "previoussyncedworkspace", previousWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "movetosyncedworkspace", moveToWorkSpace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "movetonextsyncedworkspace", moveToNextWorkspace);
    HyprlandAPI::addDispatcherV2(PHANDLE, "movetoprevioussyncedworkspace", moveToPreviousWorkspace);

    onWorkSpaceChangeHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "workspace", onWorkSpaceChange);

    return {"Custom Plugin", "A plugin to keep the workspace on both montitors synces", "Tijs", "1.0"};
}
