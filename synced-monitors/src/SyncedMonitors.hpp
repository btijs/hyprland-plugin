#pragma once

#ifndef SYNCEDWORKSPACES_H
#define SYNCEDWORKSPACES_H

typedef WORKSPACEID DESKID;

class SyncedMonitors {

public:
    static void initializeWorkspaces();

    static std::string getWorkspaceName(DESKID desk_id, MONITORID monitor_id);

    static void changeWorkspaces(DESKID desk_id);
    static void increaseWorkspaces(int num_desks);
    static void nextWorkspaces();
    static void previousWorkspaces();
    static void moveToWorkspace(DESKID desk_id);
    static DESKID translateWorkspaceToDesk(WORKSPACEID workspace_id);

private:
    static WORKSPACEID translateDeskToWorkspace(DESKID desk_id, MONITORID monitor_id);
    static PHLWORKSPACE getWorkspace(DESKID desk_id, MONITORID monitor_id);
    static PHLWORKSPACE createWorkspace(DESKID desk_id, MONITORID monitor_id);
};

#endif // SYNCEDWORKSPACES_H
