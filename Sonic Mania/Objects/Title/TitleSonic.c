#include "SonicMania.h"

ObjectTitleSonic *TitleSonic;

void TitleSonic_Update(void)
{
    RSDK_THIS(TitleSonic);
    RSDK.ProcessAnimation(&entity->animator1);
    if (entity->animator1.frameID == entity->animator1.frameCount - 1)
        RSDK.ProcessAnimation(&entity->animator2);
}

void TitleSonic_LateUpdate(void) {}

void TitleSonic_StaticUpdate(void) {}

void TitleSonic_Draw(void)
{
    RSDK_THIS(TitleSonic);
    RSDK.SetClipBounds(0, 0, 0, RSDK_screens->width, 160);
    RSDK.DrawSprite(&entity->animator1, 0, 0);
    RSDK.SetClipBounds(0, 0, 0, RSDK_screens->width, RSDK_screens->height);
    if (entity->animator1.frameID == entity->animator1.frameCount - 1)
        RSDK.DrawSprite(&entity->animator2, 0, false);
}

void TitleSonic_Create(void *data)
{
    RSDK_THIS(TitleSonic);
    RSDK.SetSpriteAnimation(TitleSonic->spriteIndex, 0, &entity->animator1, true, 0);
    RSDK.SetSpriteAnimation(TitleSonic->spriteIndex, 1, &entity->animator2, true, 0);
    if (!RSDK_sceneInfo->inEditor) {
        entity->visible   = false;
        entity->active    = ACTIVE_NEVER;
        entity->drawOrder = 4;
    }
}

void TitleSonic_StageLoad(void) { TitleSonic->spriteIndex = RSDK.LoadSpriteAnimation("Title/Sonic.bin", SCOPE_STAGE); }

#if RETRO_INCLUDE_EDITOR
void TitleSonic_EditorDraw(void) {}

void TitleSonic_EditorLoad(void) {}
#endif

void TitleSonic_Serialize(void) {}
