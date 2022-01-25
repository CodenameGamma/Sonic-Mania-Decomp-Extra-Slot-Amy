// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: ChemBubble Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectChemBubble *ChemBubble;

void ChemBubble_Update(void)
{
    RSDK_THIS(ChemBubble);
    StateMachine_Run(self->state);
}

void ChemBubble_LateUpdate(void) {}

void ChemBubble_StaticUpdate(void) {}

void ChemBubble_Draw(void)
{
    RSDK_THIS(ChemBubble);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

void ChemBubble_Create(void *data)
{
    RSDK_THIS(ChemBubble);
    if (!SceneInfo->inEditor) {
        self->visible    = true;
        self->active     = ACTIVE_NORMAL;
        self->drawOrder  = Zone->drawOrderHigh - 2;
        self->startPos.x = self->position.x;
        self->startPos.y = self->position.y;
#if RETRO_USE_PLUS
        self->velocity.y = RSDK.RandSeeded(-0x20000, 0, &Zone->randSeed);
        self->angleShift = RSDK.RandSeeded(12, 16, &Zone->randSeed);
        self->amplitude  = RSDK.RandSeeded(9, 10, &Zone->randSeed);
#else
        self->velocity.y = RSDK.Rand(-0x20000, 0);
        self->angleShift = RSDK.Rand(12, 16);
        self->amplitude  = RSDK.Rand(9, 10);
#endif
        RSDK.SetSpriteAnimation(ChemBubble->aniFrames, RSDK.Rand(1, 3), &self->animator, true, 0);
        self->drawFX = FX_SCALE;
        self->state  = ChemBubble_State_Appear;
    }
}

void ChemBubble_StageLoad(void) { ChemBubble->aniFrames = RSDK.LoadSpriteAnimation("CPZ/ChemPool.bin", SCOPE_STAGE); }

void ChemBubble_State_Appear(void)
{
    RSDK_THIS(ChemBubble);
    self->scale.x += 0x20;
    self->scale.y = self->scale.x;
    if (self->scale.x == 0x200) {
        self->drawFX = FX_NONE;
        self->state  = ChemBubble_State_Rising;
    }
}

void ChemBubble_State_Rising(void)
{
    RSDK_THIS(ChemBubble);
    EntityChemicalPool *parent = (EntityChemicalPool *)self->parent;

    self->velocity.y -= 0x2000;
    self->position.y += self->velocity.y;
    int32 x          = (RSDK.Sin256(self->position.y >> self->angleShift) << self->amplitude) + self->startPos.x;
    self->position.x = x;
    int32 deform     = ChemicalPool->surfaceDeformation[x >> 20];
    int32 y = parent->offsetY + (((x >> 12) & 0xFF) * (ChemicalPool->surfaceDeformation[(x + 0x100000) >> 20] - deform) >> 8) + deform + 0x20000;
    if (self->position.y <= y) {
        self->position.y = y;
        if (self->animator.animationID == 2) {
            RSDK.SetSpriteAnimation(ChemBubble->aniFrames, 3, &self->animator, true, 0);
            self->timer = RSDK.Rand(8, 32);
        }
        else {
            RSDK.SetSpriteAnimation(ChemBubble->aniFrames, 4, &self->animator, true, 5);
        }
        self->state = ChemBubble_State_Surfaced;
    }
}

void ChemBubble_State_Surfaced(void)
{
    RSDK_THIS(ChemBubble);
    EntityChemicalPool *parent = (EntityChemicalPool *)self->parent;

    RSDK.ProcessAnimation(&self->animator);
    int32 deform     = ChemicalPool->surfaceDeformation[self->position.x >> 20];
    self->position.y = parent->offsetY
                       + (((self->position.x >> 12) & 0xFF) * (ChemicalPool->surfaceDeformation[(self->position.x + 0x100000) >> 20] - deform) >> 8)
                       + deform + 0x20000;

    if (self->animator.animationID == 4) {
        if (self->animator.frameID == self->animator.frameCount - 1)
            destroyEntity(self);
    }
    else if (--self->timer <= 0) {
        RSDK.SetSpriteAnimation(ChemBubble->aniFrames, 4, &self->animator, true, 5);
    }
}

#if RETRO_INCLUDE_EDITOR
void ChemBubble_EditorDraw(void)
{
    RSDK_THIS(ChemBubble);

    RSDK.SetSpriteAnimation(ChemBubble->aniFrames, 1, &self->animator, true, 0);

    ChemBubble_Draw();
}

void ChemBubble_EditorLoad(void) { ChemBubble->aniFrames = RSDK.LoadSpriteAnimation("CPZ/ChemPool.bin", SCOPE_STAGE); }
#endif

void ChemBubble_Serialize(void) {}
