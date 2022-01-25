#ifndef OBJ_SSZ1SETUP_H
#define OBJ_SSZ1SETUP_H

#include "SonicMania.h"

// Object Class
struct ObjectSSZ1Setup {
    RSDK_OBJECT
    int32 field_4;
    TileLayer *layer1Ptr;
    EntitySSZ1Outro *outroPtr;
};

// Entity Class
struct EntitySSZ1Setup {
	RSDK_ENTITY
};

// Object Struct
extern ObjectSSZ1Setup *SSZ1Setup;

// Standard Entity Events
void SSZ1Setup_Update(void);
void SSZ1Setup_LateUpdate(void);
void SSZ1Setup_StaticUpdate(void);
void SSZ1Setup_Draw(void);
void SSZ1Setup_Create(void* data);
void SSZ1Setup_StageLoad(void);
void SSZ1Setup_EditorDraw(void);
void SSZ1Setup_EditorLoad(void);
void SSZ1Setup_Serialize(void);

// Extra Entity Functions
void SSZ1Setup_StageFinishCB(void);


#endif //!OBJ_SSZ1SETUP_H
