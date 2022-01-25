#ifndef OBJ_CAMERA_H
#define OBJ_CAMERA_H

#include "SonicMania.h"

// Object Class
struct ObjectCamera {
    RSDK_OBJECT
    Vector2 centerBounds;
};

// Entity Class
struct EntityCamera {
    RSDK_ENTITY
    StateMachine(state);
    Entity *targetPtr;
    int32 screenID;
    Vector2 center;
    Vector2 targetMoveVel;
    Vector2 lastPos;
    Vector2 shakePos;
    Vector2 lookPos;
    Vector2 offset;
    bool32 offsetYFlag;
    int32 centerY;
    int32 adjustY;
    int32 lerpPercent;
    int32 lerpSpeed;
    int32 lerpType;
    Vector2 endLerpPos;
    Vector2 startLerpPos;
    Vector2 boundsOffset;
    int32 boundsL;
    int32 boundsR;
    int32 boundsT;
    int32 boundsB;
};

// Object Struct
extern ObjectCamera *Camera;

// Standard Entity Events
void Camera_Update(void);
void Camera_LateUpdate(void);
void Camera_StaticUpdate(void);
void Camera_Draw(void);
void Camera_Create(void* data);
void Camera_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void Camera_EditorDraw(void);
void Camera_EditorLoad(void);
#endif
void Camera_Serialize(void);

// Extra Entity Functions
void Camera_SetCameraBounds(EntityCamera *entity);
EntityCamera *Camera_SetTargetEntity(int32 screen, void *t);
void Camera_ShakeScreen(int32 screen, int32 shakeX, int32 shakeY);
void Camera_HandleHBounds(void);
void Camera_HandleVBounds(void);
void Camera_SetupLerp(int32 type, int32 screen, int32 x, int32 y, int32 speed);

//States
void Camera_State_Roam(void);
void Camera_State_Follow(void);
void Camera_State_HLock(void);
void Camera_State_VLock(void);
void Camera_State_HandleLerp(void);

#endif //!OBJ_CAMERA_H
