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
APICALL EXPORT std::string PLUGIN_API_VERSION()
{
    return HYPRLAND_API_VERSION;
}

void onWorkSpaceChange(void *data, SCallbackInfo &info, std::any val)
{
    const auto focused_workspace = std::any_cast<PHLWORKSPACE>(val);
    SyncedMonitors::changeWorkspaces(SyncedMonitors::translateWorkspaceToDesk(focused_workspace->m_iID));
}

void focusWorkSpace(std::string arg) {
    SyncedMonitors::changeWorkspaces(stoi(arg));
}

void nextWorkSpace(std::string arg) {
    SyncedMonitors::nextWorkspaces();
}

void previousWorkSpace(std::string arg) {
    SyncedMonitors::previousWorkspaces();
}

void moveToWorkSpace(std::string arg) {
    SyncedMonitors::moveToWorkspace(stoi(arg));
}

// This function is called when the plugin is loaded.
APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle)
{
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH)
    {
        HyprlandAPI::addNotification(PHANDLE, "[CUSTOM-PLUGIN] Mismatched headers! Can't proceed.",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch");
    }

    SyncedMonitors::initializeWorkspaces();

    HyprlandAPI::addDispatcher(PHANDLE, "syncworkspace", focusWorkSpace);
    HyprlandAPI::addDispatcher(PHANDLE, "nextworkspace", nextWorkSpace);
    HyprlandAPI::addDispatcher(PHANDLE, "previousworkspace", previousWorkSpace);
    HyprlandAPI::addDispatcher(PHANDLE, "movetosyncedworkspace", moveToWorkSpace);

    onWorkSpaceChangeHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "workspace", onWorkSpaceChange);

    return {"Custom Plugin", "A plugin to keep the workspace on both montitors synces", "Tijs", "1.0"};
}