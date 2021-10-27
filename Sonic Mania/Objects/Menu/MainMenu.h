#ifndef OBJ_MAINMENU_H
#define OBJ_MAINMENU_H

#include "SonicMania.h"

#if RETRO_USE_PLUS
// Object Class
typedef struct {
    RSDK_OBJECT
    Entity *menuControlPtr;
    Entity *promptPtr;
    Entity *dioramaPtr;
} ObjectMainMenu;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityMainMenu;

// Object Struct
extern ObjectMainMenu *MainMenu;

// Standard Entity Events
void MainMenu_Update(void);
void MainMenu_LateUpdate(void);
void MainMenu_StaticUpdate(void);
void MainMenu_Draw(void);
void MainMenu_Create(void* data);
void MainMenu_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void MainMenu_EditorDraw(void);
void MainMenu_EditorLoad(void);
#endif
void MainMenu_Serialize(void);

// Extra Entity Functions
void MainMenu_Initialize(void);
bool32 MainMenu_ReturnToTitleOption(void);
void MainMenu_ExitGame(void);
void MainMenu_ExitGameOption(void);
void MainMenu_StartExitGame(void);
void MainMenu_ChangeMenu(void);

#if RETRO_USE_EGS
int32 MainMenu_BuyPlusDialogCB(void)
#endif

void MainMenu_Unknown2(void);
void MainMenu_Unknown3(void);
void MainMenu_Unknown4(void);
#endif

#endif //!OBJ_MAINMENU_H
