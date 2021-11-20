#include "SonicMania.h"

ObjectInkWipe *InkWipe;

void InkWipe_Update(void)
{
    RSDK_THIS(InkWipe);
    RSDK.ProcessAnimation(&self->animator);
}

void InkWipe_LateUpdate(void)
{

}

void InkWipe_StaticUpdate(void)
{

}

void InkWipe_Draw(void)
{
    RSDK_THIS(InkWipe);

    RSDK.DrawSprite(&self->animator, NULL, false);
}

void InkWipe_Create(void* data)
{
    RSDK_THIS(InkWipe);
    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_BOUNDS;
        self->visible       = true;
        self->drawOrder     = Zone->drawOrderHigh;
        self->updateRange.x = 0x800000;
        self->updateRange.y = 0x800000;
        RSDK.SetSpriteAnimation(InkWipe->aniFrames, 0, &self->animator, true, 0);
    }
}

void InkWipe_StageLoad(void)
{
    if (RSDK.CheckStageFolder("PSZ1"))
        InkWipe->aniFrames = RSDK.LoadSpriteAnimation("PSZ1/InkWipe.bin", SCOPE_STAGE);
}

#if RETRO_INCLUDE_EDITOR
void InkWipe_EditorDraw(void)
{

}

void InkWipe_EditorLoad(void)
{

}
#endif

void InkWipe_Serialize(void)
{

}

