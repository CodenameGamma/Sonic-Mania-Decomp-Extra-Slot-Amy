#include "RetroEngine.hpp"

#if RETRO_REV02
DummyUserStorage *userStorage = NULL;
UserDBStorage *userDBStorage  = NULL;

const char *userDebugValNames[8] = { "Ext <PLUS>", "SYSTEM_PLATFORM", "SYSTEM_REGION", "SYSTEM_LANGUAGE", "SYS_CNFRM_FLIP" };
void setupUserDebugValues()
{
    achievements->SetDebugValues();
    leaderboards->SetDebugValues();
    richPresence->SetDebugValues();
    stats->SetDebugValues();
    userStorage->SetDebugValues();

    for (int i = 0; i < userCore->debugValCnt && debugValCnt < DEBUGVAL_MAX; ++i) {
        if (i == 2 || i == 3) {
            SetDebugValue(userDebugValNames[i], userCore->values[i], DTYPE_INT8, 0, i == 2 ? REGION_EU : LANGUAGE_TC);
        }
        else {
            SetDebugValue(userDebugValNames[i], userCore->values[i], DTYPE_BOOL, false, true);
        }
    }
}
void userInitUnknown1()
{
    achievements->InitUnknown1();
    leaderboards->InitUnknown1();
    richPresence->InitUnknown1();
    stats->InitUnknown1();
    userStorage->InitUnknown1();
}
void userInitUnknown2()
{
    achievements->InitUnknown2();
    leaderboards->InitUnknown2();
    richPresence->InitUnknown2();
    stats->InitUnknown2();
    userStorage->InitUnknown2();
}
#endif

int GetUserAuthStatus() { return STATUS_OK; }
int GetUserStorageStatus()
{
#if RETRO_REV02
    return userStorage->authStatus;
#else
    return STATUS_OK;
#endif
}
#if RETRO_REV02
int UserStorageStatusUnknown1()
{
    if (userStorage->saveStatus == STATUS_ERROR)
        return STATUS_ERROR;
    else
        return userStorage->statusCode;
}
int GetSaveStatus()
{
    if (userStorage->noSaveActive)
        return STATUS_OK;
    else
        return userStorage->saveStatus;
}
void SetSaveStatusOK()
{
    if (userStorage->saveStatus == STATUS_CONTINUE)
        userStorage->saveStatus = STATUS_OK;
}
void SetSaveStatusForbidden()
{
    if (userStorage->saveStatus == STATUS_CONTINUE)
        userStorage->saveStatus = STATUS_FORBIDDEN;
}
void SetSaveStatusError()
{
    if (userStorage->saveStatus == STATUS_CONTINUE)
        userStorage->saveStatus = STATUS_ERROR;
}
void ClearUserStorageStatus() { userStorage->saveStatus = STATUS_NONE; }
void SetUserStorageStatus() { userStorage->saveStatus = STATUS_CONTINUE; }
void SetUserStorageNoSave(int state) { userStorage->noSaveActive = state; }
int GetUserStorageNoSave() { return userStorage->noSaveActive; }
#endif

int TryAuth()
{
#if RETRO_REV02
    userStorage->authStatus = STATUS_OK;
    return userStorage->authStatus;
#else
    return STATUS_OK;
#endif
}
int TryInitStorage()
{
#if RETRO_REV02
    userStorage->saveStatus = STATUS_OK;
    return userStorage->saveStatus;
#else
    return STATUS_OK;
#endif
}
bool32 GetUserName(TextInfo *info)
{
    SetText(info, (char *)"IntegerGeorge802", false);
    return true;
}
void ClearPrerollErrors()
{
#if RETRO_REV02
    if (userStorage->authStatus != STATUS_OK)
        userStorage->authStatus = STATUS_NONE;

    userStorage->field_14 = 0;
    if (userStorage->saveStatus != STATUS_OK)
        userStorage->saveStatus = STATUS_NONE;
#else
    printLog(PRINT_NORMAL, "DUMMY ClearPrerollErrors()");
#endif
}

bool32 TryLoadUserFile(const char *filename, void *buffer, uint bufSize, int (*callback)(int))
{
    bool32 success = false;
    memset(buffer, 0, bufSize);
#if RETRO_REV02
    if (!userStorage->noSaveActive) {
#endif
        success = LoadUserFile(filename, buffer, bufSize);

        if (bufSize >= 4) {
            byte *bufTest = (byte *)buffer;
            // quick and dirty zlib check
            if (bufTest[0] == 0x78 && (bufTest[1] == 0x01 || bufTest[1] == 0x9C)) {
                uLongf destLen = bufSize;

                byte *compData = NULL;
                AllocateStorage(bufSize, (void **)&compData, DATASET_TMP, false);
                memcpy(compData, buffer, bufSize);

                uncompress((Bytef *)buffer, &destLen, compData, bufSize);

                RemoveStorageEntry((void **)&compData);
            }
        }

        if (callback)
            callback(STATUS_OK);
#if RETRO_REV02
    }
    else {
        char buffer[0x100];
        sprintf(buffer, "TryLoadUserFile(%s, %p, %u, %p) failing due to noSave", filename, buffer, bufSize, callback);

        if (callback)
            callback(STATUS_ERROR);
    }
#endif

    return success;
}
bool32 TrySaveUserFile(const char *filename, void *buffer, uint bufSize, int (*callback)(int), bool32 compressData)
{
    bool32 success = false;
#if RETRO_REV02
    if (!userStorage->noSaveActive) {
#endif
        if (compressData) {
            int *cbuf = NULL;
            AllocateStorage(bufSize, (void **)&cbuf, DATASET_TMP, false);

            uLongf clen = bufSize;
            compress((Bytef *)cbuf, &clen, (Bytef *)buffer, (uLong)bufSize);

            success = SaveUserFile(filename, cbuf, clen);

            RemoveStorageEntry((void **)&cbuf);
        }
        else {
            success = SaveUserFile(filename, buffer, bufSize);
        }

        if (callback)
            callback(STATUS_OK);
#if RETRO_REV02
    }
    else {
        char buffer[0x100];
        sprintf(buffer, "TrySaveUserFile(%s, %p, %u, %p, %s) failing due to noSave", filename, buffer, bufSize, callback,
                compressData ? "true" : "false");

        if (callback)
            callback(STATUS_ERROR);
    }
#endif

    return success;
}
bool32 TryDeleteUserFile(const char *filename, int (*callback)(int))
{
#if RETRO_REV02
    if (!userStorage->noSaveActive) {
#endif
        DeleteUserFile(filename);

        if (callback)
            callback(STATUS_OK);
#if RETRO_REV02
    }
    else {
        char buffer[0x100];
        sprintf(buffer, "TryDeleteUserFile(%s, %p) failing due to noSave", filename, callback);

        if (callback)
            callback(STATUS_ERROR);
    }
#endif

    return false;
}

void (*userFileCallback)();
void (*userFileCallback2)();
char userFileDir[0x100];

bool32 LoadUserFile(const char *filename, void *buffer, uint bufSize)
{
    if (userFileCallback)
        userFileCallback();

    char pathBuffer[0x400];
    sprintf(pathBuffer, "%s%s", userFileDir, filename);
    printLog(PRINT_NORMAL, "Attempting to load user file: %s", pathBuffer);

    FileIO *file = fOpen(pathBuffer, "rb");
    if (file) {
        fSeek(file, 0, SEEK_END);
        int fSize = (int)fTell(file);
        fSeek(file, 0, SEEK_SET);
        int size = bufSize;
        if (bufSize > fSize)
            size = fSize;
        fRead(buffer, 1, size, file);
        fClose(file);
        if (userFileCallback2)
            userFileCallback2();
        return true;
    }
    else {
        if (userFileCallback2)
            userFileCallback2();
        printLog(PRINT_NORMAL, "Nope!");
    }
    return false;
}
bool32 SaveUserFile(const char *filename, void *buffer, uint bufSize)
{
    if (userFileCallback)
        userFileCallback();

    char pathBuffer[0x400];
    sprintf(pathBuffer, "%s%s", userFileDir, filename);
    printLog(PRINT_NORMAL, "Attempting to save user file: %s", pathBuffer);

    FileIO *file = fOpen(pathBuffer, "wb");
    if (file) {
        fWrite(buffer, 1, bufSize, file);
        fClose(file);

        if (userFileCallback2)
            userFileCallback2();
        return true;
    }
    else {
        if (userFileCallback2)
            userFileCallback2();
        printLog(PRINT_NORMAL, "Nope!");
    }
    return false;
}
bool32 DeleteUserFile(const char *filename)
{
    if (userFileCallback)
        userFileCallback();

    char pathBuffer[0x400];
    sprintf(pathBuffer, "%s%s", userFileDir, filename);
    printLog(PRINT_NORMAL, "Attempting to delete user file: %s", pathBuffer);
    int status = remove(pathBuffer);

    if (userFileCallback2)
        userFileCallback2();
    return status == 0;
}

#if RETRO_REV02
void InitUserStorageDB(UserDBStorage *storage)
{
    storage->loadCallback[0] = UserDBStorage_LoadCB1;
    storage->loadCallback[1] = UserDBStorage_LoadCB2;
    storage->loadCallback[2] = UserDBStorage_LoadCB3;
    storage->loadCallback[3] = UserDBStorage_LoadCB4;
    storage->loadCallback[4] = UserDBStorage_LoadCB5;
    storage->loadCallback[5] = UserDBStorage_LoadCB6;
    storage->loadCallback[6] = UserDBStorage_LoadCB7;
    storage->loadCallback[7] = UserDBStorage_LoadCB8;
    storage->saveCallback[0] = UserDBStorage_SaveCB1;
    storage->saveCallback[1] = UserDBStorage_SaveCB2;
    storage->saveCallback[2] = UserDBStorage_SaveCB3;
    storage->saveCallback[3] = UserDBStorage_SaveCB4;
    storage->saveCallback[4] = UserDBStorage_SaveCB5;
    storage->saveCallback[5] = UserDBStorage_SaveCB6;
    storage->saveCallback[6] = UserDBStorage_SaveCB7;
    storage->saveCallback[7] = UserDBStorage_SaveCB8;

    for (int i = 0; i < RETRO_USERDB_MAX; ++i) {
        memset(&storage->userDB[i], 0, sizeof(UserDB));
        storage->userDB[i].sortedRowList.Clear();
    }
}
void ReleaseUserStorageDB(UserDBStorage *storage)
{
    for (int i = 0; i < RETRO_USERDB_MAX; ++i) {
        storage->userDB[i].sortedRowList.Clear(true);
    }
}

//UserDB Management
ushort InitUserDB(const char *name, ...)
{
    int tableID = -1;
    uint uuid   = 0;
    GenerateCRC(&uuid, (char *)name);

    for (int i = 0; i < RETRO_USERDB_MAX; ++i) {
        if (!userDBStorage->userDB[i].loaded) {
            tableID = i;
            break;
        }
    }

    if (tableID == -1)
        return -1;

    va_list list;
    va_start(list, name);

    userDBStorage->userDB[tableID].loaded = true;
    userDBStorage->userDB[tableID].name   = name;
    GenerateCRC(&userDBStorage->userDB[tableID].uuid, (char *)name);
    InitUserDBValues(&userDBStorage->userDB[tableID], list);
    UserDBRefreshRowUnknown(userDBStorage->userDB[tableID].parent);
    va_end(list);
    return tableID;
}
ushort LoadUserDB(const char *filename, void (*callback)(int))
{
    int tableID = -1;
    uint uuid   = 0;
    GenerateCRC(&uuid, (char *)filename);
    for (int i = 0; i < RETRO_USERDB_MAX; ++i) {
        if (uuid == userDBStorage->userDB[i].uuid && userDBStorage->userDB[i].loaded)
            return i;
    }

    for (int i = 0; i < RETRO_USERDB_MAX; ++i) {
        if (!userDBStorage->userDB[i].loaded) {
            tableID = i;
            break;
        }
    }

    if (tableID == -1) {
        if (callback)
            callback(STATUS_ERROR);
        return -1;
    }

    AllocateStorage(sizeof(UserDB), (void **)&userDBStorage->readBuffer[tableID], DATASET_TMP, true);
    userDBStorage->userLoadCB[tableID] = UserDBLoadCB;
    userDBStorage->callbacks[tableID]  = callback;
    TryLoadUserFile(filename, userDBStorage->readBuffer[tableID], sizeof(UserDB), userDBStorage->loadCallback[tableID]);
    UserDB *userDB = &userDBStorage->userDB[tableID];
    userDB->loaded = true;
    userDB->name   = filename;
    userDB->uuid   = uuid;
    return tableID;
}
bool32 SaveUserDB(ushort tableID, void (*callback)(int))
{
    UserDB *userDB = &userDBStorage->userDB[tableID];

    bool32 success = false;
    if (userDB->active) {
        int totalSize = (int)GetUserDBWriteSize(userDB);
        AllocateStorage(totalSize, (void **)&userDBStorage->writeBuffer[tableID], DATASET_TMP, true);
        SaveDBToBuffer(userDB, totalSize, (byte *)userDBStorage->writeBuffer[tableID]);
        userDBStorage->userSaveCB[tableID] = UserDBSaveCB;
        userDBStorage->callbacks[tableID]  = callback;
        success = TrySaveUserFile(userDB->name, userDBStorage->writeBuffer[tableID], totalSize, userDBStorage->saveCallback[tableID], true);
    }
    else {
        if (callback)
            callback(STATUS_ERROR);
    }
    return success;
}

void ClearUserDB(ushort tableID)
{
    if (tableID == 0xFFFF)
        return;

    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (userDB->loaded) {
        userDB->sortedRowList.Clear();
        userDB->loaded      = false;
        userDB->active      = false;
        userDB->valid       = false;
        userDB->uuid        = 0;
        userDB->rowCount  = 0;
        userDB->columnCount = 0;
        memset(userDB->columnTypes, 0, sizeof(userDB->columnTypes));
        memset(userDB->columnNames, 0, sizeof(userDB->columnNames));
        memset(userDB->columnUUIDs, 0, sizeof(userDB->columnUUIDs));
        memset(userDB->rows, 0, sizeof(userDB->rows));
        userDB->rowsChanged = true;
    }
}
void ClearAllUserDBs()
{
    for (int i = 0; i < RETRO_USERDB_MAX; ++i) ClearUserDB(i);
}

ushort GetUserDBByID(ushort tableID, uint uuid)
{
    if (tableID == 0xFFFF)
        return -1;
    if (!uuid)
        return -1;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return -1;
    if (!userDB->rowCount)
        return -1;

    for (int i = 0; i < userDB->rowCount; ++i) {
        if (uuid == userDB->rows[i].uuid) {
            return i;
        }
    }
    return -1;
}

//UserDB Columns
bool32 AddUserDBColumn(UserDBRow *userDB, int type, char *name, void *value)
{
    UserDB *db = userDB->parent;
    uint uuid  = 0;
    GenerateCRC(&uuid, name);

    for (int c = 0; c < db->columnCount; ++c) {
        if (db->columnUUIDs[c] == uuid) {
            if (c < 0 || type != db->columnTypes[c])
                return 0;
            StoreUserDBValue(&userDB->values[c], type, value);

            time_t t;
            time(&t);
            userDB->changeTime = *localtime(&t);
            return true;
        }
    }

    return false;
}
int GetDBColumnID(UserDB *userDB, const char *name)
{
    uint uuid = 0;
    GenerateCRC(&uuid, (char *)name);

    int id = -1;
    for (int i = 0; i < userDB->columnCount; ++i) {
        if (uuid == userDB->columnUUIDs[i]) {
            id = i;
            break;
        }
    }
    return id;
}
bool32 GetUserDBColumn(UserDBRow *userDB, int type, char *name, void *value)
{
    UserDB *db = userDB->parent;
    uint uuid  = 0;
    GenerateCRC(&uuid, name);

    for (int c = 0; c < db->columnCount; ++c) {
        if (db->columnUUIDs[c] == uuid) {
            if (c < 0 || type != db->columnTypes[c])
                return 0;
            RetrieveUserDBValue(&userDB->values[c], type, value);
            return true;
        }
    }

    return false;
}

//UserDB Rows
int AddUserDBRow(ushort tableID)
{
    if (tableID == 0xFFFF)
        return -1;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return -1;

    if (userDB->rowCount >= RETRO_USERDB_ROW_MAX)
        return -1;

    UserDBRow *row = &userDB->rows[userDB->rowCount];
    row->uuid = CreateRowUUID(userDB);

    time_t timeInfo;
    time(&timeInfo);
    tm *tmA = localtime(&timeInfo);
    tm *tmB = localtime(&timeInfo);

    memcpy(&row->createTime, tmA, sizeof(tm));
    memcpy(&row->changeTime, tmB, sizeof(tm));

    memset(row->values, 0, sizeof(UserDBValue) * RETRO_USERDB_COL_MAX);
    ++userDB->rowCount;
    userDB->valid = true;
    UpdateUserDBParents(userDB);
    userDB->rowsChanged = true;
    return userDB->rowCount - 1;
}
uint RemoveDBRow(ushort tableID, uint rowID)
{
    if (tableID == 0xFFFF || rowID == 0xFFFF)
        return 0;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return 0;

    if (rowID < userDB->rowCount) {
        UserDBRow *entry = &userDB->rows[rowID];
        memset(entry, 0, sizeof(UserDBRow));

        int id = (ushort)(userDB->rowCount - rowID - 1);
        memcpy(entry, &entry[1], id * sizeof(UserDBRow));
        memset(&entry[id + 1], 0, sizeof(UserDBRow));

        --userDB->rowCount;
        userDB->valid = true;
        UpdateUserDBParents(userDB);
        userDB->rowsChanged = true;
    }
    return true;
}
bool32 RemoveAllDBRows(ushort tableID)
{
    if (tableID == 0xFFFF)
        return 0;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return 0;
    userDB->rowCount = 0;
    memset(userDB->rows, 0, sizeof(UserDBRow) * RETRO_USERDB_ROW_MAX);
    UpdateUserDBParents(userDB);
    return true;
}
uint GetDBRowUUID(ushort tableID, int rowID)
{
    if (tableID == 0xFFFF || rowID == 0xFFFF)
        return 0;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return 0;

    return userDB->rows[rowID].uuid;
}

//UserDB Row Unknowns
ushort SetupUserDBRowSorting(ushort tableID)
{
    if (tableID == 0xFFFF)
        return 0;
    UserDBRefreshRowUnknown(userDBStorage->userDB[tableID].parent);

    return userDBStorage->userDB[tableID].sortedRowCount;
}
void SetupRowSortIDs(UserDB *userDB)
{
    userDB->sortedRowCount = 0;
    memset(userDB->sortedRowIDs, 0, sizeof(userDB->sortedRowIDs));

    for (int i = 0; i < userDB->sortedRowList.Count(); ++i) {
        userDB->sortedRowIDs[i] = *userDB->sortedRowList.At(i);
        ++userDB->sortedRowCount;
    }
}
void UserDBRefreshRowUnknown(UserDB *userDB)
{
    userDB->sortedRowList.Clear();

    for (int i = 0; i < userDB->rowCount; ++i) {
        int *row = userDB->sortedRowList.Append();
        if (row)
            *row = i;
    }

    userDB->rowsChanged = false;

    SetupRowSortIDs(userDB);
}
int AddUserDBRowSortFilter(ushort tableID, int type, const char *name, void *value)
{
    if (tableID == 0xFFFF)
        return 0;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return 0;

    RemoveNonMatchingSortRows(userDB->parent, name, value);
    return userDB->sortedRowCount;
}
int SortUserDBRows(ushort tableID, int type, const char *name, bool32 active)
{
    if (tableID == 0xFFFF)
        return 0;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return 0;

    HandleUserDBSorting(userDB->parent, type, (char*)name, active);
    return userDB->sortedRowCount;
}
int GetSortedUserDBRowCount(ushort tableID)
{
    if (tableID == 0xFFFF)
        return 0;

    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active)
        return 0;

    return userDB->sortedRowCount;
}
int GetSortedUserDBRowID(ushort tableID, ushort entryID)
{
    if (tableID == 0xFFFF)
        return -1;
    UserDB *userDB = &userDBStorage->userDB[tableID];
    if (!userDB->active || userDB->rowsChanged || entryID >= userDB->sortedRowCount)
        return -1;

    return userDB->sortedRowIDs[entryID];
}

//UserDB Values
void InitUserDBValues(UserDB *userDB, va_list list)
{
    int cnt = 0;
    while (true) {
        int type = va_arg(list, int);
        if (type == 0)
            break;

        userDB->columnTypes[cnt] = type;
        memset(userDB->columnNames[cnt], 0, 0x10);
        sprintf(userDB->columnNames[cnt], "%s", va_arg(list, const char *));
        GenerateCRC(&userDB->columnUUIDs[cnt], userDB->columnNames[cnt]);
        ++cnt;
    }

    userDB->columnCount = cnt;
    userDB->rowCount  = 0;
    memset(userDB->rows, 0, sizeof(UserDBRow) * RETRO_USERDB_ROW_MAX);
    UpdateUserDBParents(userDB);
    userDB->active = true;
    userDB->valid  = true;
}
bool32 GetUserDBValue(ushort tableID, int rowID, int type, char *name, void *value)
{
    if (tableID == 0xFFFF || rowID == -1 || !userDBStorage->userDB[tableID].active)
        return false;
    return GetUserDBColumn(&userDBStorage->userDB[tableID].rows[rowID], type, name, value);
}
bool32 SetUserDBValue(ushort tableID, int rowID, int type, char *name, void *value)
{
    if (tableID == 0xFFFF || rowID == -1 || !userDBStorage->userDB[tableID].active)
        return false;

    return AddUserDBColumn(&userDBStorage->userDB[tableID].rows[rowID], type, name, value);
}
bool32 CheckDBValueMatch(UserDBValue *valueA, int row, int column)
{
    UserDB *userDB      = (UserDB *)valueA->parent;
    UserDBValue *valueB = &userDB->rows[row].values[column];

    if (valueA->data && valueB->data) {
        switch (valueA->size) {
            case 1:
            case 2:
            case 6: return memcmp(valueA->data, valueB->data, sizeof(byte)) == 0;
            case 3:
            case 7: return memcmp(valueA->data, valueB->data, sizeof(ushort)) == 0;
            case 4:
            case 8:
            case 10:
            case 15: return memcmp(valueA->data, valueB->data, sizeof(uint)) == 0;
            case 16: {
                char *string1 = (char *)valueA->data;
                char *string2 = (char *)valueA->data;
                int len1      = strlen(string1);
                int len2      = strlen(string2);
                if (len1 != len2)
                    return false;

                for (int i = 0; i < len1; ++i) {
                    if (string1[i] != string2[i])
                        return false;
                }
                return true;
            }
            default: break;
        }
    }
    return false;
}
void StoreUserDBValue(UserDBValue *value, int type, void *data)
{
    value->size = 0;
    memset(value->data, 0, sizeof(value->data));
    if (data) {
        switch (type) {
            case 1:
            case 2:
            case 6:
                value->size = sizeof(byte);
                memcpy(value->data, data, sizeof(byte));
                break;
            case 3:
            case 7:
                value->size = sizeof(ushort);
                memcpy(value->data, data, sizeof(ushort));
                break;
            case 4:
            case 8:
            case 10:
            case 15:
                value->size = sizeof(int);
                memcpy(value->data, data, sizeof(int));
                break;
            case 16: {
                char *string = (char *)data;
                int len      = strlen(string);
                if (len < 15) {
                    value->size = len + 1;
                    int id      = 0;
                    while (string[id]) {
                        value->data[id] = string[id];
                    }
                }
                break;
            }
            default: break;
        }
    }
}
void RetrieveUserDBValue(UserDBValue *value, int type, void *data)
{
    byte *valData = (byte *)data;
    switch (type) {
        case 1:
        case 2:
        case 6: memcpy(valData, value->data, sizeof(byte)); break;
        case 3:
        case 7: memcpy(valData, value->data, sizeof(ushort)); break;
        case 4:
        case 8:
        case 10:
        case 15: memcpy(valData, value->data, sizeof(int)); break;
        case 16: {
            memset(valData, 0, value->size + 1);
            char *string = (char *)data;
            for (int c = 0; c < value->size; ++c) {
                valData[c] = string[c];
            }
            break;
        }
        default: break;
    }
}

//UserDB Misc
int GetUserDBRowsChanged(ushort tableID) { return userDBStorage->userDB[tableID].rowsChanged; }
void GetUserDBCreationTime(ushort tableID, int entryID, char *buf, size_t size, char *format)
{
    if (tableID != 0xFFFF && entryID != 0xFFFF) {
        UserDB *userDB = &userDBStorage->userDB[tableID];
        if (userDB->active)
            strftime(buf, size, format, &userDB->rows[entryID].createTime);
    }
}

void UpdateUserDBParents(UserDB *userDB)
{
    userDB->parent = userDB;
    for (int r = 0; r < RETRO_USERDB_ROW_MAX; ++r) {
        userDB->rows[r].parent = userDB;
        for (int c = 0; c < userDB->columnCount; ++c) {
            userDB->rows[r].values[c].parent = &userDB->rows[r];
        }
    }
}
size_t GetUserDBWriteSize(UserDB *userDB)
{
    int colSize = 1;
    if (userDB->columnCount)
        colSize = (0x10 * userDB->columnCount) + userDB->columnCount + 1;
    size_t size = colSize + 10;

    for (int r = 0; r < RETRO_USERDB_ROW_MAX; ++r) {
        userDB->rows[r].parent = userDB;
        int rowSize            = (sizeof(tm) * 2) + sizeof(uint);
        for (int c = 0; c < userDB->columnCount; ++c) {
            rowSize += sizeof(byte) + userDB->rows[r].values[c].size;
        }
        size += rowSize;
    }

    return size;
}
bool32 LoadDBFromBuffer(UserDB *userDB, byte *buffer)
{
    uint signature = *(uint *)buffer;
    if (signature != 0x80074B1E)
        return false;
    buffer += sizeof(int);
    buffer += sizeof(int); // used size

    userDB->rowCount = *(ushort *)buffer;
    buffer += sizeof(ushort);

    userDB->columnCount = *buffer;
    buffer++;

    for (int c = 0; c < userDB->columnCount; ++c) {
        userDB->columnTypes[c] = *buffer;
        buffer++;

        sprintf(userDB->columnNames[c], "%s", (char *)buffer);
        buffer += 0x10;

        GenerateCRC(&userDB->columnUUIDs[c], userDB->columnNames[c]);
    }

    for (int r = 0; r < RETRO_USERDB_ROW_MAX; ++r) {
        userDB->rows[r].uuid = *(uint *)buffer;
        buffer += sizeof(uint);

        memcpy(&userDB->rows[r].createTime, buffer, sizeof(tm));
        buffer += sizeof(tm);
        memcpy(&userDB->rows[r].changeTime, buffer, sizeof(tm));
        buffer += sizeof(tm);

        for (int c = 0; c < userDB->columnCount; ++c) {
            userDB->rows[r].values[c].size = *buffer;
            buffer++;

            memcpy(&userDB->rows[r].values[c].data, buffer, userDB->rows[r].values[c].size);
            buffer += userDB->rows[r].values[c].size;
        }
    }

    userDB->active = true;
    UpdateUserDBParents(userDB);
    return true;
}
void SaveDBToBuffer(UserDB *userDB, int totalSize, byte *buffer)
{
    int size = 0;
    if (totalSize >= sizeof(int)) {
        size           = sizeof(int);
        *(int *)buffer = 0x80074B1E; // signature
        buffer += sizeof(int);
    }
    if (size + sizeof(int) <= totalSize) {
        size += sizeof(int);
        *(int *)buffer = (int)GetUserDBWriteSize(userDB); // used size
        buffer += sizeof(int);
    }
    if (size + sizeof(ushort) <= totalSize) {
        size += sizeof(ushort);
        *(ushort *)buffer = userDB->rowCount;
        buffer += sizeof(ushort);
    }
    if (size + sizeof(byte) <= totalSize) {
        ++size;
        *buffer++ = userDB->columnCount;
    }

    for (int c = 0; c < userDB->columnCount; ++c) {
        if (size + sizeof(byte) <= totalSize) {
            ++size;
            *buffer++ = (byte)userDB->columnTypes[c];
        }
        if (size + 0x10 <= totalSize) {
            memset(buffer, 0, 0x10 * sizeof(byte));
            sprintf((char *)buffer, "%s", userDB->columnNames[c]);
            size += 0x10;
            buffer += 0x10;
        }
    }

    for (int r = 0; r < RETRO_USERDB_ROW_MAX; ++r) {
        if (size + sizeof(uint) <= totalSize) {
            size += sizeof(uint);
            *(uint *)buffer = userDB->rows[r].uuid;
            buffer += sizeof(uint);
        }
        if (size + sizeof(tm) <= totalSize) {
            memcpy(buffer, &userDB->rows[r].createTime, sizeof(tm));
            size += sizeof(tm);
            buffer += sizeof(tm);
        }
        if (size + sizeof(tm) <= totalSize) {
            memcpy(buffer, &userDB->rows[r].changeTime, sizeof(tm));
            size += sizeof(tm);
            buffer += sizeof(tm);
        }

        for (int c = 0; c < userDB->columnCount; ++c) {
            if (size + sizeof(byte) <= totalSize) {
                ++size;
                *buffer++ = (byte)userDB->rows[r].values[c].size;
            }
            if (userDB->rows[r].values[c].size + size <= totalSize) {
                memcpy(buffer, userDB->rows[r].values[c].data, userDB->rows[r].values[c].size);
                size += userDB->rows[r].values[c].size;
                buffer += userDB->rows[r].values[c].size;
            }
        }
    }

    if (size < totalSize)
        memset(buffer, 0, totalSize - size);
}
void HandleNonMatchRowRemoval(UserDB *userDB, UserDBValue *value, int column)
{
    for (int i = userDB->sortedRowList.Count() - 1; i >= 0; --i) {
        if (!CheckDBValueMatch(value, userDB->sortedRowIDs[i], column)) {
            userDB->sortedRowList.Remove(i);
        }
    }
}
void RemoveNonMatchingSortRows(UserDB *userDB, const char *name, void *value)
{
    int id = GetDBColumnID(userDB, name);

    if (id >= 0) {
        if (userDB->sortedRowCount) {
            UserDBValue dbValue;
            // this is very hacky
            dbValue.parent = (UserDBRow *)userDB;
            StoreUserDBValue(&dbValue, userDB->columnTypes[id], value);
            HandleNonMatchRowRemoval(userDB, &dbValue, id);
            SetupRowSortIDs(userDB);
        }
    }
}

bool32 CompareUserDBValues(UserDBRow *row1, UserDBRow *row2, int size, char *name, bool32 flag)
{
    uint8 data1[0x10];
    uint8 data2[0x10];
    memset(data1, 0, sizeof(data1));
    memset(data2, 0, sizeof(data2));

    GetUserDBColumn(row1, size, name, &data1);
    GetUserDBColumn(row2, size, name, &data2);
    switch (size) {
        case 1:
        case 2: {
            uint8 value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(uint8));
            memcpy(&value2, data2, sizeof(uint8));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 6: {
            int8 value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(int8));
            memcpy(&value2, data2, sizeof(int8));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 3: {
            uint16 value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(uint16));
            memcpy(&value2, data2, sizeof(uint16));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 7: {
            int16 value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(int16));
            memcpy(&value2, data2, sizeof(int16));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 4:
        case 15: {
            uint32 value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(uint32));
            memcpy(&value2, data2, sizeof(uint32));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 8: {
            int32 value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(int32));
            memcpy(&value2, data2, sizeof(int32));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 10: {
            float value1 = 0, value2 = 0;
            memcpy(&value1, data1, sizeof(float));
            memcpy(&value2, data2, sizeof(float));

            if (!flag)
                return value1 > value2;
            else
                return value1 < value2;
            break;
        }
        case 16: {
            char *string1 = (char *)data1;
            char *string2 = (char *)data2;

            int result = strcmp(string1, string2);
            if (flag)
                return result > 0 ? true : false;
            else
                return result < 0 ? true : false;
            break;
        }
        default: break;
    }
    return false;
}

void HandleUserDBSorting(UserDB *userDB, int size, char *name, bool32 flag)
{
    if (!userDB->rowsChanged && userDB->sortedRowCount) {
        //sort by value
        if (size || name) {
            int col = GetDBColumnID(userDB, name);
            if (col >= 0) {
                for (int i = 0; i < userDB->sortedRowList.Count(); i++) {
                    for (int j = i + 1; j < userDB->sortedRowList.Count(); j++) {
                        int *id1 = userDB->sortedRowList.At(i);
                        int *id2 = userDB->sortedRowList.At(j);

                        if (CompareUserDBValues(&userDB->rows[*id1], &userDB->rows[*id2], size, name, flag)) {
                            int temp = *id1;
                            *id1     = *id2;
                            *id2     = temp;
                        }
                    }
                }  

                SetupRowSortIDs(userDB);
            }
        }
        else { //sort by date
            for (int i = 0; i < userDB->sortedRowList.Count(); i++) {
                for (int j = i + 1; j < userDB->sortedRowList.Count(); j++) {
                    int *id1 = userDB->sortedRowList.At(i);
                    int *id2 = userDB->sortedRowList.At(j);

                    double d = difftime(mktime(&userDB->rows[*id1].createTime), mktime(&userDB->rows[*id2].createTime));

                    bool32 swap = false;
                    if (flag)
                        swap = (d < 0) - (d > 0);
                    else
                        swap = (d > 0) - (d < 0);

                    if (swap) {
                        int temp = *id1;
                        *id1     = *id2;
                        *id2     = temp;
                    }
                }
            }  

            SetupRowSortIDs(userDB);
        }
    }
}
uint CreateRowUUID(UserDB *userDB)
{
    bool32 flag = true;
    uint uuid   = 0;

    while (flag) {
        byte bytes[4];
        bytes[0] = rand();
        bytes[1] = rand();
        bytes[2] = rand();
        bytes[3] = rand();
        uuid = (bytes[0]) | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);

        if (uuid < 0x10000000)
            uuid |= 0x10000000;

        flag = false;
        for (int e = 0; e < userDB->rowCount; ++e) {
            if (uuid == userDB->rows[e].uuid) {
                flag = true;
            }
        }
    }
    return uuid;
}

//User Storage CBs
int UserDBLoadCB(ushort tableID, int status)
{
    bool32 result = false;
    if (status == STATUS_OK) {
        result = LoadDBFromBuffer(&userDBStorage->userDB[tableID], (byte *)userDBStorage->readBuffer[tableID]);
        if (result) {
            UserDBRefreshRowUnknown(userDBStorage->userDB[tableID].parent);
        }
    }
    else {
        ClearUserDB(tableID);
    }
    RemoveStorageEntry((void **)&userDBStorage->readBuffer[tableID]);

    if (userDBStorage->callbacks[tableID]) {
        userDBStorage->callbacks[tableID](result ? status : STATUS_ERROR);
        userDBStorage->callbacks[tableID] = NULL;
    }
    return result;
}
int UserDBSaveCB(ushort tableID, int status)
{
    RemoveStorageEntry((void **)&userDBStorage->writeBuffer[tableID]);
    if (status != STATUS_OK)
        userDBStorage->userDB[tableID].valid = false;

    if (userDBStorage->callbacks[tableID]) {
        userDBStorage->callbacks[tableID](status);
        userDBStorage->callbacks[tableID] = NULL;
        return 1;
    }
    return 0;
}

int UserDBStorage_LoadCB1(int status)
{
    if (userDBStorage->userLoadCB[0])
        return userDBStorage->userLoadCB[0](0, status);
    return 0;
}
int UserDBStorage_LoadCB2(int status)
{
    if (userDBStorage->userLoadCB[1])
        return userDBStorage->userLoadCB[1](1, status);
    return 0;
}
int UserDBStorage_LoadCB3(int status)
{
    if (userDBStorage->userLoadCB[2])
        return userDBStorage->userLoadCB[2](2, status);
    return 0;
}
int UserDBStorage_LoadCB4(int status)
{
    if (userDBStorage->userLoadCB[3])
        return userDBStorage->userLoadCB[3](3, status);
    return 0;
}
int UserDBStorage_LoadCB5(int status)
{
    if (userDBStorage->userLoadCB[4])
        return userDBStorage->userLoadCB[4](4, status);
    return 0;
}
int UserDBStorage_LoadCB6(int status)
{
    if (userDBStorage->userLoadCB[5])
        return userDBStorage->userLoadCB[5](5, status);
    return 0;
}
int UserDBStorage_LoadCB7(int status)
{
    if (userDBStorage->userLoadCB[6])
        return userDBStorage->userLoadCB[6](6, status);
    return 0;
}
int UserDBStorage_LoadCB8(int status)
{
    if (userDBStorage->userLoadCB[7])
        return userDBStorage->userLoadCB[7](7, status);
    return 0;
}

int UserDBStorage_SaveCB1(int status)
{
    if (userDBStorage->userSaveCB[0])
        return userDBStorage->userSaveCB[0](0, status);
    return 0;
}
int UserDBStorage_SaveCB2(int status)
{
    if (userDBStorage->userSaveCB[1])
        return userDBStorage->userSaveCB[1](1, status);
    return 0;
}
int UserDBStorage_SaveCB3(int status)
{
    if (userDBStorage->userSaveCB[2])
        return userDBStorage->userSaveCB[2](2, status);
    return 0;
}
int UserDBStorage_SaveCB4(int status)
{
    if (userDBStorage->userSaveCB[3])
        return userDBStorage->userSaveCB[3](3, status);
    return 0;
}
int UserDBStorage_SaveCB5(int status)
{
    if (userDBStorage->userSaveCB[4])
        return userDBStorage->userSaveCB[4](4, status);
    return 0;
}
int UserDBStorage_SaveCB6(int status)
{
    if (userDBStorage->userSaveCB[5])
        return userDBStorage->userSaveCB[5](5, status);
    return 0;
}
int UserDBStorage_SaveCB7(int status)
{
    if (userDBStorage->userSaveCB[6])
        return userDBStorage->userSaveCB[6](6, status);
    return 0;
}
int UserDBStorage_SaveCB8(int status)
{
    if (userDBStorage->userSaveCB[7])
        return userDBStorage->userSaveCB[7](7, status);
    return 0;
}

#endif