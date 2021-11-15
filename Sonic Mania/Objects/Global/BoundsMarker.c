#include "SonicMania.h"

ObjectBoundsMarker *BoundsMarker;

void BoundsMarker_Update(void)
{
    RSDK_THIS(BoundsMarker);

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
        BoundsMarker_CheckBounds(player, entity, false);
    }
}

void BoundsMarker_LateUpdate(void) {}

void BoundsMarker_StaticUpdate(void) {}

void BoundsMarker_Draw(void) {}

void BoundsMarker_Create(void *data)
{
    RSDK_THIS(BoundsMarker);

    if (!RSDK_sceneInfo->inEditor) {
        if (entity->vsDisable && globals->gameMode == MODE_COMPETITION) {
            destroyEntity(entity);
        }
        else {
            entity->active = ACTIVE_XBOUNDS;
            if (entity->width)
                entity->width = entity->width << 15;
            else
                entity->width = 0x180000;
            entity->updateRange.x += entity->width << 15;

            for (int32 p = 0; p < Player->playerCount; ++p) {
                EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
                BoundsMarker_CheckBounds(player, entity, true);
            }
        }
    }
}

void BoundsMarker_StageLoad(void) {}

void BoundsMarker_CheckBounds(void *p, EntityBoundsMarker *entity, bool32 setPos)
{
    EntityPlayer *player = (EntityPlayer *)p;
    uint16 playerID      = RSDK.GetEntityID(player);
    if (Player_CheckValidState(player) || player->objectID == DebugMode->objectID) {
        if (abs(entity->position.x - player->position.x) < entity->width) {
            switch (entity->type) {
                case 0: // bottom
                    Zone->screenBoundsB2[playerID] = entity->position.y;
                    Zone->screenBoundsB1[playerID] = Zone->screenBoundsB2[playerID] >> 0x10;
                    Zone->deathBoundary[playerID]  = entity->position.y;
                    break;
                case 1: // bottom (offset)
                    if (player->position.y < entity->position.y - (entity->offset << 16)) {
                        Zone->screenBoundsB2[playerID] = entity->position.y;
                        Zone->screenBoundsB1[playerID] = Zone->screenBoundsB2[playerID] >> 0x10;
                        Zone->deathBoundary[playerID]  = entity->position.y;
                    }
                    break;
                case 2: // top (offset)
                    if (player->position.y > entity->position.y + (entity->offset << 16)) {
                        Zone->screenBoundsT2[playerID] = entity->position.y;
                        Zone->screenBoundsT1[playerID] = Zone->screenBoundsT2[playerID] >> 0x10;
                    }
                    break;
                case 3: // top
                    Zone->screenBoundsT2[playerID] = entity->position.y;
                    Zone->screenBoundsT1[playerID] = Zone->screenBoundsT2[playerID] >> 0x10;
                    break;
                default: break;
            }
        }

        if (setPos) {
            EntityCamera *camera = player->camera;
            if (camera) {
                camera->boundsL = Zone->screenBoundsL1[playerID];
                camera->boundsR = Zone->screenBoundsR1[playerID];
                camera->boundsT = Zone->screenBoundsT1[playerID];
                camera->boundsB = Zone->screenBoundsB1[playerID];
            }
        }
    }
}
void BoundsMarker_CheckAllBounds(void *p, bool32 setPos)
{
    EntityPlayer *player = (EntityPlayer *)p;
    if (Player_CheckValidState(player) || player->objectID == DebugMode->objectID) {
        foreach_all(BoundsMarker, entity) { BoundsMarker_CheckBounds(player, entity, setPos); }
    }
}

#if RETRO_INCLUDE_EDITOR
void BoundsMarker_EditorDraw(void)
{
    RSDK_THIS(BoundsMarker);
    Animator animator;
    RSDK.SetSpriteAnimation(BoundsMarker->spriteIndex, 0, &animator, true, 2);
    RSDK.DrawSprite(&animator, NULL, false);

    Vector2 drawPos;

    int w = entity->width << 16;
    if (!entity->width)
        w = 24 << 16;

    //Bounds
    RSDK.DrawLine(entity->position.x - w, entity->position.y, entity->position.x + w, entity->position.y, 0xFFFF00, 0xFF, INK_NONE, false);
    if (entity->type == 1) {
        RSDK.DrawLine(entity->position.x + w, entity->position.y - (entity->offset << 16), entity->position.x + w,
                      entity->position.y - (entity->offset << 16), 0xFFFF00, 0x80, INK_BLEND, false);
    }
    else if (entity->type == 2) {
        RSDK.DrawLine(entity->position.x + w, entity->position.y + (entity->offset << 16), entity->position.x + w,
                      entity->position.y + (entity->offset << 16), 0xFFFF00, 0x80, INK_BLEND, false);
    }
}

void BoundsMarker_EditorLoad(void) { BoundsMarker->spriteIndex = RSDK.LoadSpriteAnimation("Editor/EditorIcons.bin", SCOPE_STAGE); }
#endif

void BoundsMarker_Serialize(void)
{
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_UINT8, type);
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_ENUM, width);
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_BOOL, vsDisable);
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_ENUM, offset);
}
