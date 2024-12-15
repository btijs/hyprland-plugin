#include <iostream>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/debug/Log.hpp>

#include "SyncedMonitors.hpp"

// Create an initial workspace on all monitors
// This function is called when the plugin is loaded.
void SyncedMonitors::initializeWorkspaces() {
    Debug::log(INFO, "Initializing workspaces");
    std::cout << "Initializing workspaces" << std::endl;
    for (const auto & m_vMonitor : g_pCompositor->m_vMonitors) {
        PHLWORKSPACE workspace = m_vMonitor->activeWorkspace;
        workspace->rename(
            getWorkspaceName(0, m_vMonitor->ID)
        );
        g_pEventManager->postEvent(SHyprIPCEvent{"renameworkspace", std::to_string(workspace->m_iID) + ',' + workspace->m_szName});
    }
}

std::string SyncedMonitors::getWorkspaceName(DESKID desk_id, MONITORID monitor_id) {
    return std::to_string(desk_id) + "." + std::to_string(monitor_id);
}

WORKSPACEID SyncedMonitors::translateDeskToWorkspace(DESKID desk_id, MONITORID monitor_id) {
    int num_monitors = g_pCompositor->m_vMonitors.size();
    return desk_id * num_monitors + monitor_id + 1; // Workspace IDs start at 1
}

DESKID SyncedMonitors::translateWorkspaceToDesk(WORKSPACEID workspace_id) {
    int num_monitors = g_pCompositor->m_vMonitors.size();
    return (workspace_id - 1) / num_monitors; // Workspace IDs start at 1
}

PHLWORKSPACE SyncedMonitors::getWorkspace(DESKID desk_id, const MONITORID monitor_id) {
    Debug::log(INFO, "Getting workspace for desk " + std::to_string(desk_id) + " on monitor " + std::to_string(monitor_id));
    WORKSPACEID workspace_id = translateDeskToWorkspace(desk_id, monitor_id);
    PHLWORKSPACE workspace = g_pCompositor->getWorkspaceByID(workspace_id);
    if (!workspace) {
        workspace = createWorkspace(desk_id, monitor_id);
    }
    Debug::log(INFO, "Got workspace with ID " + std::to_string(workspace_id));
    return workspace;
}

PHLWORKSPACE SyncedMonitors::createWorkspace(const DESKID desk_id, const MONITORID monitor_id) {
    WORKSPACEID workspace_id = translateDeskToWorkspace(desk_id, monitor_id);
    Debug::log(INFO, "Creating new workspace on monitor " + std::to_string(monitor_id) + " with ID " + std::to_string(workspace_id));
    std::cout << "Creating new workspace on monitor " << monitor_id << " with ID " << workspace_id << std::endl;
    PHLWORKSPACE workspace = g_pCompositor->createNewWorkspace(
        workspace_id,
        monitor_id,
        getWorkspaceName(desk_id, monitor_id)
    );
    g_pEventManager->postEvent(SHyprIPCEvent{"workspace", workspace->m_szName});
    return workspace;
}

// Change to workspace to specified desk_id on all monitors
void SyncedMonitors::changeWorkspaces(DESKID desk_id) {
    Debug::log(INFO, "Changing to desk " + std::to_string(desk_id));
    std::cout << "Changing to desk " << desk_id << std::endl;

    // Loop over all monitors and change the workspace to the specified workspace
    for (const auto & m_vMonitor : g_pCompositor->m_vMonitors) {
        PHLWORKSPACE workspace = getWorkspace(desk_id, m_vMonitor->ID);
        // Check if the workspace is already active
        // to avoid infinite loop of callbacks
        if (m_vMonitor->activeWorkspaceID() != workspace->m_iID) {
            m_vMonitor->changeWorkspace(workspace);
            g_pEventManager->postEvent(SHyprIPCEvent{"workspace", workspace->m_szName});
        }
    }
}

void SyncedMonitors::increaseWorkspaces(int num_desks) {
    const WORKSPACEID w_id = g_pCompositor->getMonitorFromCursor()->activeWorkspace->m_iID;
    const DESKID desk_id = translateWorkspaceToDesk(w_id);
    if (desk_id + num_desks < 0) {
        Debug::log(INFO, "Cannot decrease workspaces below 0");
        std::cout << "Cannot decrease workspaces below 0" << std::endl;
        return;
    }
    changeWorkspaces(desk_id  + num_desks);
}

void SyncedMonitors::nextWorkspaces() {
    increaseWorkspaces(1);
}

void SyncedMonitors::previousWorkspaces() {
    increaseWorkspaces(-1);
}

// Move active window to workspace specified by desk_id
// And change workspace to desk_id on all monitors
void SyncedMonitors::moveToWorkspace(DESKID desk_id) {
    Debug::log(INFO, "Moving active window to desk " + std::to_string(desk_id));
    std::cout << "Moving active window to desk " << desk_id << std::endl;

    PHLWORKSPACE target_workspace = getWorkspace(desk_id, g_pCompositor->getMonitorFromCursor()->ID);
    PHLWINDOW active_window = g_pCompositor->m_pLastWindow.lock();
    if (!active_window) {
        Debug::log(ERR, "No active window found");
        std::cout << "No active window found" << std::endl;
        return;
    }
    g_pCompositor->moveWindowToWorkspaceSafe(active_window, target_workspace);

    changeWorkspaces(desk_id);
}

void SyncedMonitors::moveToIncreaseWorkspaces(int num_desks) {
    const WORKSPACEID w_id = g_pCompositor->getMonitorFromCursor()->activeWorkspace->m_iID;
    const DESKID desk_id = translateWorkspaceToDesk(w_id);
    if (desk_id + num_desks < 0) {
        Debug::log(INFO, "Cannot decrease workspaces below 0");
        std::cout << "Cannot decrease workspaces below 0" << std::endl;
        return;
    }
    moveToWorkspace(desk_id + num_desks);
}

void SyncedMonitors::moveToNextWorkspace() {
    moveToIncreaseWorkspaces(1);
}

void SyncedMonitors::moveToPreviousWorkspace() {
    moveToIncreaseWorkspaces(-1);
}