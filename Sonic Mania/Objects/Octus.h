#ifndef OBJ_OCTUS_H
#define OBJ_OCTUS_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectOctus;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityOctus;

// Object Struct
extern ObjectOctus *Octus;

// Standard Entity Events
void Octus_Update();
void Octus_LateUpdate();
void Octus_StaticUpdate();
void Octus_Draw();
void Octus_Create(void* data);
void Octus_StageLoad();
void Octus_EditorDraw();
void Octus_EditorLoad();
void Octus_Serialize();

// Extra Entity Functions


#endif //!OBJ_OCTUS_H