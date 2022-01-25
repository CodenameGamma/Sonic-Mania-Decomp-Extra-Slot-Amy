#ifndef OBJ_UIVIDEO_H
#define OBJ_UIVIDEO_H

#include "SonicMania.h"

// Object Class
struct ObjectUIVideo {
    RSDK_OBJECT
    bool32 playing;
    uint16 aniFrames; // prolly used in-editor
};

// Entity Class
struct EntityUIVideo {
    RSDK_ENTITY
    StateMachine(state);
    TextInfo videoFile1;
    TextInfo videoFile2;
    TextInfo audioFile;
    bool32 gotoStage;
    TextInfo stageListCategory;
    TextInfo stageListName;
    int32 timer;
    Animator unusedAnimator;
};

// Object Struct
extern ObjectUIVideo *UIVideo;

// Standard Entity Events
void UIVideo_Update(void);
void UIVideo_LateUpdate(void);
void UIVideo_StaticUpdate(void);
void UIVideo_Draw(void);
void UIVideo_Create(void* data);
void UIVideo_StageLoad(void);
#if RETRO_INCLUDE_EDITOR
void UIVideo_EditorDraw(void);
void UIVideo_EditorLoad(void);
#endif
void UIVideo_Serialize(void);

// Extra Entity Functions
bool32 UIVideo_SkipCB(void);

void UIVideo_State_PlayVideo1(void);
void UIVideo_State_PlayVideo2(void);
void UIVideo_State_FinishPlayback(void);

#endif //!OBJ_UIVIDEO_H
