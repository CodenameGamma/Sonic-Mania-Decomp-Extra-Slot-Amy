#ifndef OBJ_PAUSEMENU_H
#define OBJ_PAUSEMENU_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 sfxBleep;
    uint16 sfxAccept;
    bool32 disableEvents;
    bool32 controllerDisconnect;
    bool32 dword10;
    bool32 signoutDetected;
#if RETRO_USE_PLUS
    bool32 plusChanged;
#endif
    bool32 channelFlags[0x10];
    uint16 lookupTable[0x10000];
} ObjectPauseMenu;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    StateMachine(stateDraw);
    int32 timer;
    int32 field_64;
    Vector2 field_68;
    Vector2 field_70;
    Entity *manager;
    uint8 triggerPlayer;
    bool32 disableRestart;
    int32 buttonCount;
    uint8 buttonIDs[3];
    void (*buttonActions[3])(void);
    Entity* buttonPtrs[3];
    int32 field_A4;
    int32 fillTimer;
    bool32 (*field_AC)(void);
    int32 field_B0;
    Animator animator;
    void (*funcPtrUnknown)(void);
    int32 field_D0;
    int32 field_D4;
} EntityPauseMenu;

// Object Struct
extern ObjectPauseMenu *PauseMenu;

// Standard Entity Events
void PauseMenu_Update(void);
void PauseMenu_LateUpdate(void);
void PauseMenu_StaticUpdate(void);
void PauseMenu_Draw(void);
void PauseMenu_Create(void* data);
void PauseMenu_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void PauseMenu_EditorDraw(void);
void PauseMenu_EditorLoad(void);
#endif
void PauseMenu_Serialize(void);

// Extra Entity Functions
uint8 PauseMenu_GetPlayerCount(void);
void PauseMenu_SetupLookupTable(void);
void PauseMenu_Unknown3(void);
void PauseMenu_HandleButtonPositions(void);
void PauseMenu_AddButton(uint8 id, void *action);
void PauseMenu_SetupMenu(void);
void PauseMenu_ClearButtons(EntityPauseMenu *entity);
void PauseMenu_Unknown8(void);
void PauseMenu_Unknown9(void);
void PauseMenu_FocusCamera(void);
void PauseMenu_UpdateCameras(void);
void PauseMenu_Resume_CB(void);
void PauseMenu_Restart_CB(void);
void PauseMenu_RestartDialog_YesCB(void);
void PauseMenu_Exit_CB(void);
void PauseMenu_ExitDialog_YesCB(void);
void PauseMenu_Unknown16(void);
void PauseMenu_PauseSound(void);
void PauseMenu_ResumeSound(void);
void PauseMenu_StopSound(void);
void PauseMenu_SetupButtons(void);
void PauseMenu_Unknown21(void);
void PauseMenu_Unknown22(void);
void PauseMenu_Unknown23(void);
void PauseMenu_Unknown24(void);
void PauseMenu_Unknown26(void);
void PauseMenu_Unknown27(void);
void PauseMenu_Unknown28(void);
void PauseMenu_Unknown29(void);
void PauseMenu_Unknown31(void);
bool32 PauseMenu_Unknown32(void);
void PauseMenu_Unknown33(void);
void PauseMenu_Unknown34(void);
void PauseMenu_Unknown35(void);
void PauseMenu_Unknown36(void);
void PauseMenu_Unknown37(void);

#endif //!OBJ_PAUSEMENU_H
