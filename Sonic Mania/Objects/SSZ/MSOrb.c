#include "SonicMania.h"

ObjectMSOrb *MSOrb;

void MSOrb_Update(void)
{
    RSDK_THIS(MSOrb);
    StateMachine_Run(self->state);
}

void MSOrb_LateUpdate(void)
{

}

void MSOrb_StaticUpdate(void)
{

}

void MSOrb_Draw(void)
{
    RSDK_THIS(MSOrb);
    StateMachine_Run(self->stateDraw);
}

void MSOrb_Create(void* data)
{
    RSDK_THIS(MSOrb);

    if (!SceneInfo->inEditor) {
        self->active        = ACTIVE_NORMAL;
        self->inkEffect     = INK_ALPHA;
        self->visible       = true;
        self->updateRange.x = 0x400000;
        self->updateRange.y = 0x400000;
        self->alpha         = 0xC0;
        self->drawOrder     = Zone->drawOrderLow;
        RSDK.SetSpriteAnimation(MSOrb->aniFrames, 15, &self->animator, true, 0);
        self->state     = MSOrb_State_Orb;
        self->stateDraw = MSOrb_StateDraw_Orb;
    }
}

void MSOrb_StageLoad(void)
{
    if (RSDK.CheckStageFolder("SSZ2"))
        MSOrb->aniFrames = RSDK.LoadSpriteAnimation("SSZ2/MetalSonic.bin", SCOPE_STAGE);
    MSOrb->hitbox.left   = -6;
    MSOrb->hitbox.top    = -6;
    MSOrb->hitbox.right  = 6;
    MSOrb->hitbox.bottom = 6;
}

void MSOrb_CheckPlayerCollisions(void)
{
    RSDK_THIS(MSOrb);

    foreach_active(Player, player)
    {
        if (Player_CheckCollisionTouch(player, self, &MSOrb->hitbox))
            Player_CheckHit(player, self);
    }
}

void MSOrb_State_Orb(void)
{
    RSDK_THIS(MSOrb);

    RSDK.ProcessAnimation(&self->animator);
    self->position.x += self->velocity.x;
    self->position.y += self->velocity.y;
    MSOrb_CheckPlayerCollisions();
}

void MSOrb_StateDraw_Orb(void)
{
    RSDK_THIS(MSOrb);
    RSDK.DrawSprite(&self->animator, NULL, false);
}

#if RETRO_INCLUDE_EDITOR
void MSOrb_EditorDraw(void)
{

}

void MSOrb_EditorLoad(void)
{

}
#endif

void MSOrb_Serialize(void)
{

}

