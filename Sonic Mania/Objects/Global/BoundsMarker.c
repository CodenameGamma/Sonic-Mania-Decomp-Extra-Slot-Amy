#include "SonicMania.h"

ObjectBoundsMarker *BoundsMarker;

void BoundsMarker_Update(void)
{
    RSDK_THIS(BoundsMarker);

    for (int32 p = 0; p < Player->playerCount; ++p) {
        EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
        BoundsMarker_CheckBounds(player, self, false);
    }
}

void BoundsMarker_LateUpdate(void) {}

void BoundsMarker_StaticUpdate(void) {}

void BoundsMarker_Draw(void) {}

void BoundsMarker_Create(void *data)
{
    RSDK_THIS(BoundsMarker);

    if (!SceneInfo->inEditor) {
        if (self->vsDisable && globals->gameMode == MODE_COMPETITION) {
            destroyEntity(self);
        }
        else {
            self->active = ACTIVE_XBOUNDS;
            if (self->width)
                self->width = self->width << 15;
            else
                self->width = 0x180000;
            self->updateRange.x += self->width << 15;

            for (int32 p = 0; p < Player->playerCount; ++p) {
                EntityPlayer *player = RSDK_GET_ENTITY(p, Player);
                BoundsMarker_CheckBounds(player, self, true);
            }
        }
    }
}

void BoundsMarker_StageLoad(void) {}

void BoundsMarker_CheckBounds(void *p, EntityBoundsMarker *self, bool32 setPos)
{
    EntityPlayer *player = (EntityPlayer *)p;
    uint16 playerID      = RSDK.GetEntityID(player);
    if (Player_CheckValidState(player) || player->objectID == DebugMode->objectID) {
        if (abs(self->position.x - player->position.x) < self->width) {
            switch (self->type) {
                case 0: // bottom
                    Zone->screenBoundsB2[playerID] = self->position.y;
                    Zone->screenBoundsB1[playerID] = Zone->screenBoundsB2[playerID] >> 0x10;
                    Zone->deathBoundary[playerID]  = self->position.y;
                    break;
                case 1: // bottom (offset)
                    if (player->position.y < self->position.y - (self->offset << 16)) {
                        Zone->screenBoundsB2[playerID] = self->position.y;
                        Zone->screenBoundsB1[playerID] = Zone->screenBoundsB2[playerID] >> 0x10;
                        Zone->deathBoundary[playerID]  = self->position.y;
                    }
                    break;
                case 2: // top (offset)
                    if (player->position.y > self->position.y + (self->offset << 16)) {
                        Zone->screenBoundsT2[playerID] = self->position.y;
                        Zone->screenBoundsT1[playerID] = Zone->screenBoundsT2[playerID] >> 0x10;
                    }
                    break;
                case 3: // top
                    Zone->screenBoundsT2[playerID] = self->position.y;
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
    RSDK.SetSpriteAnimation(BoundsMarker->aniFrames, 0, &animator, true, 2);
    RSDK.DrawSprite(&animator, NULL, false);

    Vector2 drawPos;

    int w = self->width << 16;
    if (!self->width)
        w = 24 << 16;

    //Bounds
    RSDK.DrawLine(self->position.x - w, self->position.y, self->position.x + w, self->position.y, 0xFFFF00, 0xFF, INK_NONE, false);
    if (self->type == 1) {
        RSDK.DrawLine(self->position.x + w, self->position.y - (self->offset << 16), self->position.x + w,
                      self->position.y - (self->offset << 16), 0xFFFF00, 0x80, INK_BLEND, false);
    }
    else if (self->type == 2) {
        RSDK.DrawLine(self->position.x + w, self->position.y + (self->offset << 16), self->position.x + w,
                      self->position.y + (self->offset << 16), 0xFFFF00, 0x80, INK_BLEND, false);
    }
}

void BoundsMarker_EditorLoad(void) { BoundsMarker->aniFrames = RSDK.LoadSpriteAnimation("Editor/EditorIcons.bin", SCOPE_STAGE); }
#endif

void BoundsMarker_Serialize(void)
{
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_UINT8, type);
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_ENUM, width);
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_BOOL, vsDisable);
    RSDK_EDITABLE_VAR(BoundsMarker, VAR_ENUM, offset);
}
