#include "SonicMania.h"

ObjectRingField *RingField;

void RingField_Update(void)
{
    RSDK_THIS(RingField);
    if (entity->running) {
        bool32 flag = false;
        foreach_active(Player, player)
        {
            if (Player_CheckCollisionTouch(player, entity, &entity->hitbox)) {
                flag = true;
            }
        }
        if (flag) {
            if (entity->timer <= 0) {
                Vector2 pos;
                RingField_GetPos(&pos);
                EntityRing *ring          = CREATE_ENTITY(Ring, &pos, pos.x, pos.y);
                ring->animator.animationSpeed = 512;
                ring->state               = Ring_State_Normal;
                ring->drawOrder           = Zone->drawOrderLow;
                ring->stateDraw           = Ring_StateDraw_Normal;
                ring->moveType            = RING_MOVE_NONE;
                RSDK.SetSpriteAnimation(RingField->spriteIndex, 0, &ring->animator, true, 0);

                int32 sx = (RSDK_screens->centerX + RSDK_screens->position.x) << 16;
                int32 sy = (RSDK_screens->position.y + RSDK_screens->centerY) << 16;
                int32 x  = (RSDK.Rand(-RSDK_screens->centerX, RSDK_screens->centerX) << 15) + sx;
                int32 y  = RSDK.Rand(-RSDK_screens->centerY, RSDK_screens->centerY);

                int32 angle = RSDK.ATan2(x - pos.x, (y << 15) + sy - pos.y);
                ring->velocity.x = RSDK.Cos256(angle) << 9;
                ring->velocity.y = RSDK.Sin256(angle) << 9;
                entity->timer    = (entity->fluctuation * RSDK.Sin256(Zone->timer) >> 8) + entity->frequency;
            }
            else {
                entity->timer--;
            }
        }
    }
}

void RingField_LateUpdate(void) {}

void RingField_StaticUpdate(void)
{
    Vector2 updateRange;
    updateRange.x = 0x400000;
    updateRange.y = 0x400000;

    foreach_all(Ring, ring)
    {
        if (ring->velocity.x || ring->velocity.y) {
            ring->position.x += ring->velocity.x;
            ring->position.y += ring->velocity.y;
        }
        if (!RSDK.CheckOnScreen(ring, &updateRange))
            destroyEntity(ring);
    }
}

void RingField_Draw(void) {}

void RingField_Create(void *data)
{
    RSDK_THIS(RingField);

    entity->active        = ACTIVE_NORMAL;
    entity->drawOrder     = Zone->drawOrderLow;
    entity->startPos.x    = entity->position.x;
    entity->startPos.y    = entity->position.y;
    entity->visible       = false;
    entity->drawFX        = FX_FLIP;
    entity->updateRange.x = 0x800000;
    entity->updateRange.y = 0x800000;
    if (!entity->size.x)
        entity->size.x = 0x6400000;
    if (!entity->size.y)
        entity->size.y = 0x1E00000;
    if (!entity->frequency)
        entity->frequency = 60;
    if (!entity->fluctuation)
        entity->fluctuation = 20;

    entity->hitbox.left   = -entity->size.x >> 17;
    entity->hitbox.top    = -entity->size.y >> 17;
    entity->hitbox.right  = entity->size.x >> 17;
    entity->hitbox.bottom = entity->size.y >> 17;
}

void RingField_StageLoad(void) { RingField->spriteIndex = RSDK.LoadSpriteAnimation("Global/Ring.bin", SCOPE_STAGE); }

void RingField_GetPos(Vector2 *pos)
{
    int32 rand = RSDK.Rand(0, 16) % 4;
    int32 x    = 0;
    int32 y    = 0;
    switch (rand) {
        case 0:
        case 1:
            y = RSDK.Rand(-RSDK_screens->height, RSDK_screens->height) << 15;
            switch (rand) {
                case 0: x += -0x100000 - (RSDK_screens->centerX << 16); break;
                case 1: x += (RSDK_screens->centerX + 16) << 16; break;
                case 2: y += -0x100000 - (RSDK_screens->centerY << 16); break;
                case 3: y += (RSDK_screens->centerY + 16) << 16; break;
                default: break;
            }
            break;
        case 2:
        case 3:
            x = RSDK.Rand(-RSDK_screens->width, RSDK_screens->width) << 15;
            switch (rand) {
                case 0: x += -0x100000 - (RSDK_screens->centerX << 16); break;
                case 1: x += (RSDK_screens->centerX + 16) << 16; break;
                case 2: y += -0x100000 - (RSDK_screens->centerY << 16); break;
                case 3: y += (RSDK_screens->centerY + 16) << 16; break;
                default: break;
            }
            break;
        default: break;
    }
    pos->x = x + ((RSDK_screens->position.x + RSDK_screens->centerX) << 16);
    pos->y = y + ((RSDK_screens->position.y + RSDK_screens->centerY) << 16);
}

#if RETRO_INCLUDE_EDITOR
void RingField_EditorDraw(void)
{
    RSDK_THIS(RingField);
    Vector2 drawPos;

    drawPos.x = entity->position.x;
    drawPos.y = entity->position.y;
    drawPos.x -= entity->size.x >> 1;
    drawPos.y -= entity->size.y >> 1;
    RSDK.DrawLine(drawPos.x - 0x10000, drawPos.y - 0x10000, drawPos.x + entity->size.x, drawPos.y - 0x10000, 0xFFFF00, 0, INK_NONE, false);
    RSDK.DrawLine(drawPos.x - 0x10000, entity->size.y + drawPos.y, drawPos.x + entity->size.x, entity->size.y + drawPos.y, 0xFFFF00, 0, INK_NONE,
                  false);
    RSDK.DrawLine(drawPos.x - 0x10000, drawPos.y - 0x10000, drawPos.x - 0x10000, drawPos.y + entity->size.y, 0xFFFF00, 0, INK_NONE, false);
    RSDK.DrawLine(drawPos.x + entity->size.x, drawPos.y - 0x10000, drawPos.x + entity->size.x, drawPos.y + entity->size.y, 0xFFFF00, 0, INK_NONE,
                  false);
}

void RingField_EditorLoad(void) {}
#endif

void RingField_Serialize(void)
{
    RSDK_EDITABLE_VAR(RingField, VAR_VECTOR2, size);
    RSDK_EDITABLE_VAR(RingField, VAR_ENUM, frequency);
    RSDK_EDITABLE_VAR(RingField, VAR_ENUM, fluctuation);
    RSDK_EDITABLE_VAR(RingField, VAR_BOOL, running);
}
