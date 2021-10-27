#include "SonicMania.h"

#if RETRO_USE_PLUS
ObjectDialogRunner *DialogRunner;

void DialogRunner_Update(void)
{
    RSDK_THIS(DialogRunner);
    StateMachine_Run(entity->state);
}

void DialogRunner_LateUpdate(void) {}

void DialogRunner_StaticUpdate(void) {}

void DialogRunner_Draw(void) {}

void DialogRunner_Create(void *data)
{
    RSDK_THIS(DialogRunner);
    entity->active   = ACTIVE_ALWAYS;
    entity->visible  = false;
    entity->state    = (Type_StateMachine)data;
    entity->timer    = 0;
    entity->field_88 = 0;
}

void DialogRunner_StageLoad(void)
{
    DialogRunner->field_4  = 0;
    DialogRunner->field_8  = 0;
    DialogRunner->field_10 = 0;
    DialogRunner->field_C  = 0;
    DialogRunner->field_14 = 0;
    if (!globals->hasPlusInitial) {
        globals->lastHasPlus    = API.CheckDLC(DLC_PLUS);
        globals->hasPlusInitial = true;
    }
    DialogRunner->entityPtr = NULL;
    SaveGame_LoadSaveData();
    TimeAttackData->status   = 0;
    TimeAttackData->uuid     = 0;
    TimeAttackData->rowID  = -1;
    TimeAttackData->dbRank   = 0;
    TimeAttackData->rank     = 0;
    TimeAttackData->dword1C  = 0;
    Options->state           = 0;
    if (sku_platform && sku_platform != PLATFORM_DEV) {
        EntityOptions *options   = (EntityOptions *)globals->optionsRAM;
        options->vSync           = false;
        options->windowed        = false;
        options->windowBorder    = false;
        options->tripleBuffering = false;
    }
    else {
        Options_Reload();
    }
}

void DialogRunner_HandleCallback(void)
{
    RSDK_THIS(DialogRunner);
    if (entity->timer <= 0) {
        LogHelpers_Print("Callback: %x", entity->callback);
        if (entity->callback)
            entity->callback();
        destroyEntity(entity);
    }
    else {
        entity->timer--;
    }
}

void DialogRunner_NotifyAutoSave_CB(void)
{
    DialogRunner->field_14    = 0;
    globals->notifiedAutosave = true;
    UIWaitSpinner_Wait2();
}

void DialogRunner_NotifyAutoSave(void)
{
    TextInfo info;
    INIT_TEXTINFO(info);

    RSDK_THIS(DialogRunner);
    if (DialogRunner->field_14) {
        if (!UIDialog->activeDialog) {
            Localization_GetString(&info, STR_AUTOSAVENOTIF);
            EntityUIDialog *dialog = UIDialog_CreateActiveDialog(&info);
            dialog->field_B8       = 1;
            UIDialog_AddButton(2, dialog, DialogRunner_NotifyAutoSave_CB, 1);
            UIDialog_Setup(dialog);
        }
    }
    else {
        DialogRunner->entityPtr = NULL;
        destroyEntity(entity);
    }
}

void DialogRunner_SetNoSaveDisabled(void)
{
    API.SetSaveStatusForbidden();
    API.SetUserStorageNoSave(false);
}

void DialogRunner_SetNoSaveEnabled(void)
{
    API.SetSaveStatusError();
    API.SetUserStorageNoSave(true);
}

void DialogRunner_PromptSavePreference_CB(void)
{
    TextInfo info;
    INIT_TEXTINFO(info);

    RSDK_THIS(DialogRunner);
    if (API.GetSaveStatus() == STATUS_CONTINUE) {
        if (!UIDialog->activeDialog) {
            int32 stringID = STR_SAVELOADFAIL;
            switch (entity->status) {
                case STATUS_ERROR:
                    stringID = STR_NOXBOXPROFILE;
                    if (sku_platform != PLATFORM_XB1)
                        stringID = STR_SAVELOADFAIL;
                    break;
                case STATUS_CORRUPT: stringID = STR_CORRUPTSAVE; break;
                case STATUS_NOSPACE: stringID = (sku_platform == PLATFORM_XB1) + STR_NOSAVESPACE; break;
            }
            Localization_GetString(&info, stringID);
            EntityUIDialog *dialog = UIDialog_CreateActiveDialog(&info);
            dialog->field_B8       = 1;
            UIDialog_AddButton(1, dialog, DialogRunner_SetNoSaveDisabled, 1);
            UIDialog_AddButton(0, dialog, DialogRunner_SetNoSaveEnabled, 1);
            UIDialog_Setup(dialog);
        }
    }
    else {
        DialogRunner->entityPtr = NULL;
        destroyEntity(entity);
    }
}
void DialogRunner_CheckUserAuth_CB(int32 a1, int32 a2)
{
    RSDK_THIS(DialogRunner);
    if (entity->timer) {
        if (DialogRunner->field_8) {
            if (!UIDialog->activeDialog) {
                if (Zone && Zone_GetZoneID() != -1) {
                    RSDK.SetScene("Presentation", "Title Screen");
                    Zone_StartFadeOut(10, 0x000000);
                }
                else if (MenuSetup) {
                    ManiaModeMenu_StartReturnToTitle();
                }
                else if (UFO_Setup) {
                    UFO_Setup->resetToTitle = true;
                    foreach_all(UFO_Setup, setup)
                    {
                        setup->fadeColour = 0;
                        setup->state      = UFO_Setup_Unknown12;
                        setup->active     = ACTIVE_ALWAYS;
                        setup->visible    = true;
                    }
                }
                else if (FXFade) {
                    RSDK.SetScene("Presentation", "Title Screen");
                    EntityFXFade *fxFade    = CREATE_ENTITY(FXFade, NULL, entity->position.x, entity->position.y);
                    fxFade->active          = ACTIVE_ALWAYS;
                    fxFade->timer           = 0;
                    fxFade->speedIn         = 16;
                    fxFade->speedOut        = 16;
                    fxFade->state           = FXFade_State_FadeIn;
                    fxFade->drawOrder       = DRAWLAYER_COUNT - 1;
                    fxFade->isPermanent     = true;
                    fxFade->oneWay          = true;
                    fxFade->transitionScene = true;
                }
                DialogRunner->entityPtr = NULL;
                destroyEntity(entity);
            }
        }
        else if (!UIDialog->activeDialog) {
            TextInfo info;
            int32 id = STR_SIGNOUTDETECTED;
            if (entity->field_88)
                id = STR_RETRURNINGTOTITLE;
            Localization_GetString(&info, id);
            EntityUIDialog *dialog = UIDialog_CreateActiveDialog(&info);
            dialog->field_B8       = 1;
            UIDialog_AddButton(2, dialog, DialogRunner_Unknown14, 1);
            UIDialog_Setup(dialog);
        }
    }
    else {
        EntityUIDialog *dialog = UIDialog->activeDialog;
        if (dialog) {
            if (dialog->state != UIDialog_Unknown13) {
                dialog->parent->selectionDisabled = true;
                dialog->field_5C                  = 0;
                dialog->state                     = UIDialog_Unknown13;
                dialog->curCallback               = 0;
            }
        }
        else {
            if (UIControl) {
                if (UIControl_GetUIControl())
                    UIControl_Unknown6(UIControl_GetUIControl());
            }
            RSDK.SetGameMode(ENGINESTATE_FROZEN);
            RSDK.StopChannel(Music->channelID);
            entity->timer = 1;
        }
    }
}
void DialogRunner_ManageNotifs(int32 a1)
{
    RSDK_THIS(DialogRunner);
    if (GameProgress_CountUnreadNotifs()) {
        TextInfo info;
        INIT_TEXTINFO(info);
        if (!UIDialog->activeDialog) {
            int32 str = GameProgress_GetNotifStringID(GameProgress_GetNextNotif());
            Localization_GetString(&info, str);
            EntityUIDialog *dialog = UIDialog_CreateActiveDialog(&info);
            dialog->field_B4       = 1;
            dialog->field_B8       = 1;
            UIDialog_AddButton(2, dialog, DialogRunner_GetNextNotif, 1);
            UIDialog_Setup(dialog);
        }
    }
    else {
        DialogRunner->entityPtr = NULL;
        UIWaitSpinner_Wait();
        GameProgress_TrackGameProgress(DialogRunner_Wait);
        RSDK.ResetEntityPtr(entity, TYPE_BLANK, 0);
    }
}
void DialogRunner_Wait(int32 success) { UIWaitSpinner_Wait2(); }
void DialogRunner_GetNextNotif(void)
{
    if (RSDK_sceneInfo->inEditor || API.GetUserStorageNoSave() || globals->saveLoaded != STATUS_OK) {
        LogHelpers_Print("WARNING GameProgress Attempted to save before loading SaveGame file");
        return;
    }
    else {
        EntityGameProgress *progress = GameProgress_GetGameProgress();
        int32 id             = GameProgress_GetNextNotif();
        if (id >= 0)
            progress->unreadNotifs[id] = true;
    }
}
bool32 DialogRunner_CheckUnreadNotifs(void)
{
    if (!GameProgress_CountUnreadNotifs())
        return false;
    if (!DialogRunner->entityPtr)
        DialogRunner->entityPtr = CREATE_ENTITY(DialogRunner, DialogRunner_ManageNotifs, 0, 0);
    return true;
}
bool32 DialogRunner_NotifyAutosave(void)
{
    if (!DialogRunner->field_14 && !DialogRunner->entityPtr) {
        return false;
    }

    if (!DialogRunner->entityPtr || (!globals->notifiedAutosave && !DialogRunner->field_14)) {
        UIWaitSpinner_Wait();
        DialogRunner->field_14    = true;
        globals->notifiedAutosave = false;
        LogHelpers_Print("DUMMY NotifyAutosave()");
        EntityDialogRunner *dialogRunner = CREATE_ENTITY(DialogRunner, DialogRunner_NotifyAutoSave, 0, 0);
        dialogRunner->active             = ACTIVE_ALWAYS;
        DialogRunner->entityPtr          = dialogRunner;
    }
    return true;
}
void DialogRunner_GetUserAuthStatus(void)
{
    if (API.GetUserAuthStatus() == STATUS_FORBIDDEN) {
        if (DialogRunner->field_4)
            return;
        EntityDialogRunner *dialogRunner = CREATE_ENTITY(DialogRunner, DialogRunner_CheckUserAuth_CB, 0, 0);
        dialogRunner->active             = ACTIVE_ALWAYS;
        DialogRunner->entityPtr          = dialogRunner;
        DialogRunner->field_4            = true;
    }

    if (API.CheckDLC(DLC_PLUS) != globals->lastHasPlus && !DialogRunner->field_4) {
        EntityDialogRunner *dialogRunner = CREATE_ENTITY(DialogRunner, DialogRunner_CheckUserAuth_CB, 0, 0);
        dialogRunner->active             = ACTIVE_ALWAYS;
        dialogRunner->field_88           = 1;
        DialogRunner->entityPtr          = dialogRunner;
        DialogRunner->field_4            = true;
        globals->lastHasPlus             = API.CheckDLC(DLC_PLUS);
    }
}
void DialogRunner_PromptSavePreference(int32 id)
{
    if (API.GetUserStorageNoSave()) {
        LogHelpers_Print("PromptSavePreference() returning due to noSave");
        return;
    }
    LogHelpers_Print("PromptSavePreference()");
    if (API.GetSaveStatus() == STATUS_CONTINUE) {
        LogHelpers_Print("WARNING PromptSavePreference() when prompt already in progress.");
    }
    API.SetUserStorageStatus();
    EntityDialogRunner *dialogRunner = CREATE_ENTITY(DialogRunner, DialogRunner_PromptSavePreference_CB, 0, 0);
    dialogRunner->status             = id;
    DialogRunner->entityPtr          = dialogRunner;
}
void DialogRunner_Unknown14(void)
{
    DialogRunner->field_8 = 1;
}

#if RETRO_INCLUDE_EDITOR
void DialogRunner_EditorDraw(void) {}

void DialogRunner_EditorLoad(void) {}
#endif

void DialogRunner_Serialize(void) {}
#endif
