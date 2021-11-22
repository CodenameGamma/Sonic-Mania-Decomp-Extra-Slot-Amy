#include "SonicMania.h"

ObjectTimeAttackData *TimeAttackData;

void TimeAttackData_Update(void) {}
void TimeAttackData_LateUpdate(void) {}
void TimeAttackData_StaticUpdate(void) {}
void TimeAttackData_Draw(void) {}
void TimeAttackData_Create(void *data) {}
void TimeAttackData_StageLoad(void) {}

void TimeAttackData_TrackActClear(uint8 act, uint8 zone, StatInfo * stat, uint8 charID, int32 time, int32 rings, int32 score)
{
#if RETRO_USE_PLUS
    stat->statID = 0;
    stat->name   = "ACT_CLEAR";
    memset(stat->data, 0, sizeof(stat->data));
    stat->data[0] = (void *)ZoneNames[zone];
    stat->data[1] = (void *)ActNames[act];
    stat->data[2] = (void *)PlayerNames[charID];
    stat->data[3] = 0;
    stat->data[4] = intToVoid(time);
    stat->data[5] = intToVoid(rings);
    stat->data[6] = intToVoid(score);
#else
    APICallback_TrackActClear(zone, act, charID, score, rings, time);
#endif
}
void TimeAttackData_TrackTAClear(uint8 actID, uint8 zone, StatInfo *stat, uint8 charID, int32 gameMode, int32 time)
{
#if RETRO_USE_PLUS
    stat->statID = 1;
    stat->name   = "TA_CLEAR";
    memset(stat->data, 0, sizeof(stat->data));
    stat->data[0] = (void *)ZoneNames[zone];
    stat->data[1] = (void *)ActNames[actID];
    stat->data[2] = (void *)PlayerNames[charID];
    stat->data[3] = (void *)ModeNames[gameMode];
    stat->data[4] = intToVoid(time);
#else
    APICallback_TrackTAClear(zone, actID, charID, time);
#endif
}
void TimeAttackData_TrackEnemyDefeat(uint8 actID, uint8 zoneID, StatInfo *stat, uint8 charID, bool32 encore, int32 x, int32 y)
{
#if RETRO_USE_PLUS
    stat->statID   = 2;
    stat->name   = "ENEMY_DEFEAT";
    memset(stat->data, 0, sizeof(stat->data));
    stat->data[0]     = (void *)ZoneNames[zoneID];
    stat->data[1]     = (void *)ActNames[actID];
    stat->data[2]     = (void *)PlayerNames[charID];
    stat->data[3]     = intToVoid(encore);
    stat->data[4]     = intToVoid(x);
    stat->data[5]     = intToVoid(y);
#else
    APICallback_TrackEnemyDefeat(zoneID, actID, charID, x, y);
#endif
}

void TimeAttackData_ClearOptions(void)
{
    EntityMenuParam *param = (EntityMenuParam *)globals->menuParam;
    param->selectionFlag   = 0;
    memset(param->menuTag, 0, 0x100);
    param->selectionID = 0;
    param->unused1     = 0;
    param->clearFlag   = 0;
    param->zoneID      = 0;
    param->actID       = 0;
#if RETRO_USE_PLUS
    param->timeAttackRank    = 0;
    globals->gameMode = MODE_MANIA;
#else
    param->timeScore  = 0;
    globals->gameMode = MODE_NOSAVE;
#endif
    globals->suppressAutoMusic = false;
    globals->suppressTitlecard = false;
}
int32 TimeAttackData_GetManiaListPos(int32 zoneID, int32 playerID, int32 act)
{
    int32 listPos = 0;
    switch (zoneID) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5: listPos = act + 2 * zoneID; break;
        case 6: listPos = act + (2 * zoneID + 1); break;
        case 7:
            if (act)
                listPos = 2 * zoneID + 3;
            else
                listPos = (playerID == 3) + 1 + 2 * zoneID;
            break;
        case 8:
        case 9: listPos = act + 2 * (zoneID + 1); break;
        case 10:
        case 11: listPos = act + (2 * zoneID + 3); break;
        default: break;
    }
    LogHelpers_Print("playerID = %d, zoneID = %d, act = %d", playerID, zoneID, act);
    LogHelpers_Print("listPos = %d", listPos);
    return listPos;
}
int32 TimeAttackData_GetEncoreListPos(int32 zoneID, int32 playerID, int32 act)
{
    int32 listPos = 0;
    switch (zoneID) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5: listPos = act + 2 * zoneID; break;
        case 6:
        case 7:
        case 8:
        case 9: listPos = act + 2 * zoneID + 1; break;
        case 10:
        case 11: listPos = act + 2 * (zoneID + 1); break;
        default: break;
    }
    LogHelpers_Print("playerID = %d, zoneID = %d, act = %d", playerID, zoneID, act);
    LogHelpers_Print("listPos = %d", listPos);
    return listPos;
}

void TimeAttackData_GetTimeFromValue(int32 time, int32 *minsPtr, int32 *secsPtr, int32 *millisecsPtr)
{
    int32 m;
    int32 s;
    int32 ms;
    m  = time / 6000;
    s  = time % 6000 / 100;
    ms = time % 100;
    if (minsPtr)
        *minsPtr = m;
    if (secsPtr)
        *secsPtr = s;
    if (millisecsPtr)
        *millisecsPtr = ms;
}
#if RETRO_USE_PLUS
int32 TimeAttackData_LoadCB(int32 statusCode)
{
    if (statusCode == STATUS_OK) {
        globals->taTableLoaded = STATUS_OK;
        API.SetupUserDBRowSorting(globals->taTableID);
        LogHelpers_Print("Load Succeeded! Replay count: %d", API.GetSortedUserDBRowCount(globals->taTableID));
    }
    else {
        LogHelpers_Print("Load Failed! Creating new Time Attack DB");
        TimeAttackData_ResetTimeAttackDB();
    }
    LogHelpers_Print("Replay DB Slot => %d, Load Status => %d", globals->taTableID, globals->taTableLoaded);

    if (TimeAttackData->loadCallback) {
        Entity *entStore = SceneInfo->entity;
        if (TimeAttackData->loadEntityPtr)
            SceneInfo->entity = TimeAttackData->loadEntityPtr;
        TimeAttackData->loadCallback(statusCode == STATUS_OK);
        SceneInfo->entity        = entStore;
        TimeAttackData->loadCallback  = NULL;
        TimeAttackData->loadEntityPtr = NULL;
    }
    return 1;
}

void TimeAttackData_ResetTimeAttackDB(void)
{
    uint16 id = API.InitUserDB("TimeAttackDB.bin", 2, "zoneID", 2, "act", 2, "characterID", 2, "encore", 4, "score", 4, "replayID", 0, NULL);
    globals->taTableID = id;
    if (id == 0xFFFF) {
        globals->taTableLoaded = STATUS_ERROR;
    }
    else {
        globals->taTableLoaded = STATUS_OK;
        if (!API.GetUserStorageNoSave()) {
            if (globals->saveLoaded == STATUS_OK)
                TimeAttackData_MigrateLegacyTADB();
        }
    }
}

void TimeAttackData_MigrateLegacyTADB(void)
{
    if (globals->saveLoaded == STATUS_OK) {
        TimeAttackData->dword1C = 1;
        LogHelpers_Print("===========================");
        LogHelpers_Print("Migrating Legacy TA Data...");
        LogHelpers_Print("===========================");

        for (int32 zone = 0; zone < 12; ++zone) {
            for (int32 act = 0; act < 2; ++act) {
                for (int32 charID = 0; charID < 3; ++charID) {
                    int32 off = 12;
                    for (int32 rank = 0; rank < 3; ++rank) {
                        uint16 *saveRAM = NULL;
                        if (globals->saveLoaded == STATUS_OK)
                            saveRAM = (uint16 *)&globals->saveRAM[0x800];
                        else
                            saveRAM = NULL;
                        int32 pos = act + 2 * (off + zone) - 10;
                        if (saveRAM[pos + rank + 2 * pos]) {
                            int32 time = saveRAM[pos + rank + 2 * pos];
                            LogHelpers_Print("Import: zone=%d act=%d charID=%d rank=%d -> %d", zone, act, charID + 1, rank, time);
                            TimeAttackData_AddTADBEntry(zone, charID + 1, act, MODE_MANIA, time, NULL);
                        }
                    }
                    off += 12;
                }
            }

            TimeAttackData->dword1C = 0;
        }
    }
}

int32 TimeAttackData_AddTimeAttackDBEntry(char zone, char charID, int32 act, char mode, int32 time)
{
    if (globals->taTableLoaded != STATUS_OK)
        return -1;

    uint16 rowID = API.AddUserDBRow(globals->taTableID);
    int32 encore   = mode & 1;
    API.SetUserDBValue(globals->taTableID, rowID, 2, "zoneID", &zone);
    API.SetUserDBValue(globals->taTableID, rowID, 2, "act", &act);
    API.SetUserDBValue(globals->taTableID, rowID, 2, "characterID", &charID);
    API.SetUserDBValue(globals->taTableID, rowID, 2, "encore", &encore);
    API.SetUserDBValue(globals->taTableID, rowID, 4, "score", &time);
    API.SetUserDBValue(globals->taTableID, rowID, 4, "replayID", NULL);
    uint32 uuid = API.GetUserDBRowUUID(globals->taTableID, rowID);
    char buf[0x20];
    memset(buf, 0, 0x20 * sizeof(char));
    API.GetUserDBCreationTime(globals->taTableID, rowID, buf, 23, "%Y/%m/%d %H:%M:%S");
    LogHelpers_Print("Time Attack DB Added Entry");
    LogHelpers_Print("Created at %s", buf);
    LogHelpers_Print("Row ID: %d", rowID);
    LogHelpers_Print("UUID: %08X", uuid);
    return rowID;
}

int32 TimeAttackData_AddTADBEntry(char zone, char charID, int32 act, int32 mode, int32 time, void (*callback)(int32))
{
    uint16 row = TimeAttackData_AddTimeAttackDBEntry(zone, charID, act, mode, time);
    uint32 uuid  = API.GetUserDBRowUUID(globals->taTableID, row);
    TimeAttackData_ConfigureTableView(zone, charID, act, mode);
    if (API.GetSortedUserDBRowCount(globals->taTableID) > 3) {
        int32 row = API.GetSortedUserDBRowID(globals->taTableID, 3);
        API.RemoveDBRow(globals->taTableID, row);
        TimeAttackData_ConfigureTableView(zone, charID, act, mode);
    }

    int32 c     = 0;
    int32 rowID = 0;
    for (c = 0; c < 3; ++c) {
        rowID = API.GetSortedUserDBRowID(globals->taTableID, c);
        if (API.GetUserDBRowUUID(globals->taTableID, rowID) == uuid)
            break;
    }
    if (c == 3) {
        if (callback)
            callback(0);
        return 0;
    }

    TimeAttackData->uuid   = uuid;
    TimeAttackData->rowID  = rowID;
    TimeAttackData->dbRank = c + 1;
    if (TimeAttackData->dword1C == 1) {
        if (callback) {
            callback(1);
            return c + 1;
        }
    }
    else {
        TimeAttackData_SaveTimeAttackDB(callback);
    }
    return c + 1;
}

int32 TimeAttackData_SaveTimeAttackDB(void (*callback)(int32))
{
    if (API.GetUserStorageNoSave() || globals->taTableID == 0xFFFF || globals->taTableLoaded != STATUS_OK) {
        if (callback)
            callback(0);
    }
    else {
        LogHelpers_Print("Saving Time Attack DB");
        TimeAttackData->saveEntityPtr = SceneInfo->entity;
        TimeAttackData->saveCallback  = callback;
        API.SaveUserDB(globals->taTableID, TimeAttackData_SaveTimeAttackDB_CB);
    }
    return 1;
}

void TimeAttackData_SaveTimeAttackDB_CB(int32 statusCode)
{
    if (TimeAttackData->saveCallback) {
        Entity *entStore = SceneInfo->entity;
        if (TimeAttackData->saveEntityPtr)
            SceneInfo->entity = TimeAttackData->saveEntityPtr;
        TimeAttackData->saveCallback(statusCode == STATUS_OK);
        SceneInfo->entity        = entStore;
        TimeAttackData->saveCallback  = NULL;
        TimeAttackData->saveEntityPtr = NULL;
    }
}

int32 TimeAttackData_GetScore(uint8 zone, uint8 charID, uint8 act, int32 encore, int32 rank)
{
    if (rank > 3 && rank)
        return 0;

    uint8 rankID = rank - 1;
    if (!TimeAttackData->status || charID != TimeAttackData->characterID || zone != TimeAttackData->zoneID || act != TimeAttackData->act
        || encore != TimeAttackData->encore) {
        TimeAttackData_ConfigureTableView(zone, charID, act, encore);
    }

    int32 row = API.GetSortedUserDBRowID(globals->taTableID, rankID);
    if (row == -1)
        return 0;
    
    int32 score = 0;
    API.GetUserDBValue(globals->taTableID, row, 4, "score", &score);
    return score;
}

int32 TimeAttackData_GetReplayID(uint8 zone, uint8 charID, uint8 act, int32 encore, int32 rank)
{
    if (rank >= 3 && rank)
        return 0;

    uint8 rankID = rank - 1;
    if (!TimeAttackData->status || charID != TimeAttackData->characterID || zone != TimeAttackData->zoneID
        || act != TimeAttackData->act || encore != TimeAttackData->encore) {
        TimeAttackData_ConfigureTableView(zone, charID, act, encore);
    }
    
    int32 row = API.GetSortedUserDBRowID(globals->taTableID, rankID);
    if (row != -1) {
        int32 replayID = 0;
        API.GetUserDBValue(globals->taTableID, row, 4, "replayID", &replayID);
        return replayID;
    }
    return 0;
}

void TimeAttackData_ConfigureTableView(uint8 zoneID, uint8 characterID, uint8 act, int32 encore)
{
    LogHelpers_Print("ConfigureTableView(%d, %d, %d, %d)", characterID, zoneID, act, encore);
    // setup every sort row ID for every entry
    API.SetupUserDBRowSorting(globals->taTableID);
    //remove any sort row IDs that dont match the following values
    API.AddRowSortFilter(globals->taTableID, 2, "zoneID", &zoneID);
    API.AddRowSortFilter(globals->taTableID, 2, "act", &act);
    API.AddRowSortFilter(globals->taTableID, 2, "characterID", &characterID);
    API.AddRowSortFilter(globals->taTableID, 2, "encore", &encore);
    //sort the remaining rows
    API.SortDBRows(globals->taTableID, 4, "score", false);
    TimeAttackData->status      = 1;
    TimeAttackData->zoneID      = zoneID;
    TimeAttackData->act         = act;
    TimeAttackData->characterID = characterID;
    TimeAttackData->encore      = encore & 1;
}

void TimeAttackData_GetLeaderboardRank_CB(int32 status, int32 rank)
{
    if (status) {
        LogHelpers_Print("Got back leaderboard rank: %d. Not bad!", rank);
        TimeAttackData->rank = rank;
    }
}

void TimeAttackData_AddLeaderboardEntry(uint8 zone, char playerID, uint8 act, int32 mode, int32 time)
{
    StatInfo stat; 
    TimeAttackData_TrackTAClear(act, zone, &stat, playerID, mode, time);
    API.TryTrackStat(&stat);

    const char *leaderboardName = "";
    if (zone < 12 && act < 2 && playerID <= 5) {
        int32 id = 10 * zone + 5 * act + (playerID - 1);
        if (mode)
            id += 120;
        leaderboardName = LeaderboardNames[id];
    }
    API.TrackScore(leaderboardName, time, TimeAttackData_GetLeaderboardRank_CB);
}

#else
void TimeAttackData_SaveTATime(uint8 zone, uint8 act, uint8 player, uint8 rank, uint16 time)
{
    rank--;
    if (rank < 3) {
        // playerID * (1 zones)
        // zone * (1 acts)
        // act * (3 ranks)
        uint16 *record = TimeAttackData_GetRecordedTime(zone, act, player, 1);

        for (int32 r = 2; r > rank; --r) {
            record[r] = record[r - 1];
        }

        record[rank] = time;
    }
}

uint16 *TimeAttackData_GetRecordedTime(uint8 zone, uint8 act, uint8 player, uint8 rank) {
    rank--;
    if (rank >= 3)
        return NULL;

    uint16 *saveRAM = NULL;
    if (globals->saveLoaded == STATUS_OK)
        saveRAM = (uint16 *)&globals->saveRAM[0x800];
    else
        return NULL;

    int32 pos = act + 2 * (zone * 12) - 10;
    return &saveRAM[pos + rank + 2 * pos];
}

#endif

void TimeAttackData_EditorDraw(void) {}
void TimeAttackData_EditorLoad(void) {}
void TimeAttackData_Serialize(void) {}
