#ifndef DUMMY_PRESENCE_H
#define DUMMY_PRESENCE_H

namespace RSDK
{

namespace SKU
{

#if RETRO_REV02

// This is the "dummy" struct, it serves as the base in the event a suitable API isn't loaded (such as in this decomp)
// This struct should never be removed, other structs such as "SteamRichPresence" would be added and "richPresence" would be set to that instead
struct DummyRichPresence : UserRichPresence {
    void SetPresence(byte id, TextInfo *info);
};
#endif

} // namespace SKU

} // namespace RSDK

#endif // DUMMY_PRESENCE_H
