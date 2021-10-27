#include "SonicMania.h"

ObjectAnimalHBH *AnimalHBH;

void AnimalHBH_Update(void)
{
    RSDK_THIS(AnimalHBH);
    entity->position.x -= 0x10000;
    int32 pos = entity->position.x - (RSDK_screens->position.x << 16) - (RSDK_screens->centerX << 16);
    if (pos < 0)
        pos = ((RSDK_screens->position.x + RSDK_screens->centerX) << 16) - entity->position.x;
    if (pos < 0x800000)
        AnimalHBH->palCnt = 0x20 * entity->type;
    if (!entity->type) {
        ++entity->timer;
        if ((RSDK_controller->keyA.press || RSDK_controller->keyStart.press))
            entity->timer = 1400;
        if (entity->timer == 1400) {
            EntityFXFade *fxFade = CREATE_ENTITY(FXFade, NULL, entity->position.x, entity->position.y);
            fxFade->speedIn      = 12;
            fxFade->wait         = 240;
        }
        if (entity->timer == 1536) {
            RSDK.SetScene("Presentation", "Menu");
            RSDK.LoadScene();
        }

        if (AnimalHBH->palID < AnimalHBH->palCnt) {
            AnimalHBH->palID++;
        }

        RSDK.SetPaletteEntry(1, 1, RSDK.GetPaletteEntry(5, AnimalHBH->palID));
    }
}

void AnimalHBH_LateUpdate(void) {}

void AnimalHBH_StaticUpdate(void) {}

void AnimalHBH_Draw(void)
{
    RSDK_THIS(AnimalHBH);
    Vector2 drawPos;

    drawPos.x = 2 * (entity->position.x - ((RSDK_screens->position.x + RSDK_screens->centerX) << 15));
    drawPos.y = entity->position.y;

    RSDK.SetActivePalette(1, 0, RSDK_screens->height);
    RSDK.DrawSprite(&entity->silhouetteData, &drawPos, 0);

    RSDK.SetActivePalette(4, 0, RSDK_screens->height);
    RSDK.DrawSprite(&entity->animalHBHData, NULL, 0);

    RSDK.SetActivePalette(1, 0, RSDK_screens->height);
}

void AnimalHBH_Create(void *data)
{
    RSDK_THIS(AnimalHBH);
    if (!RSDK_sceneInfo->inEditor) {
        entity->visible       = true;
        entity->drawOrder     = 2;
        entity->active        = ACTIVE_NORMAL;
        entity->updateRange.x = 0x800000;
        entity->updateRange.y = 0x800000;
        RSDK.SetSpriteAnimation(AnimalHBH->animalHBHSprite, entity->type, &entity->animalHBHData, true, 0);
        RSDK.SetSpriteAnimation(AnimalHBH->silhouetteSprite, entity->type, &entity->silhouetteData, true, 0);
    }
}

void AnimalHBH_StageLoad(void)
{
    AnimalHBH->animalHBHSprite  = RSDK.LoadSpriteAnimation("Credits/AnimalHBH.bin", SCOPE_STAGE);
    AnimalHBH->silhouetteSprite = RSDK.LoadSpriteAnimation("Credits/Silhouettes.bin", SCOPE_STAGE);
    AnimalHBH->palID            = 0;
    AnimalHBH->palCnt           = 0;
}

#if RETRO_INCLUDE_EDITOR
void AnimalHBH_EditorDraw(void) {}

void AnimalHBH_EditorLoad(void) {}
#endif

void AnimalHBH_Serialize(void) { RSDK_EDITABLE_VAR(AnimalHBH, VAR_UINT8, type); }
