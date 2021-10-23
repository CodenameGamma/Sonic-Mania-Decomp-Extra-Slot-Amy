#include "RetroEngine.hpp"

ushort rIndexes[0x100];
ushort gIndexes[0x100];
ushort bIndexes[0x100];

ushort globalPalette[PALETTE_COUNT][PALETTE_SIZE];
ushort activeGlobalRows[PALETTE_COUNT];
ushort activeStageRows[PALETTE_COUNT];
ushort stagePalette[PALETTE_COUNT][PALETTE_SIZE];

ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];

byte gfxLineBuffer[SCREEN_YSIZE];

int maskColour = 0;
ushort *lookupTable = NULL;

#if RETRO_HARDWARE_RENDER
uint gfxPalette16to32[0x10000];
#endif

#if RETRO_REV02
void LoadPalette(byte paletteID, const char *filename, ushort rowFlags)
{
    char buffer[0x80];    
    sprintf(buffer, "Data/Palettes/%s", filename);

    FileInfo info;
    InitFileInfo(&info);
    if (LoadFile(&info, buffer, FMODE_RB)) {
        for (int r = 0; r < 0x10; ++r) {
            if (!(rowFlags >> r & 1)) {
                for (int c = 0; c < 0x10; ++c) {
                    byte red                             = ReadInt8(&info);
                    byte green                           = ReadInt8(&info);
                    byte blue                            = ReadInt8(&info);
                    fullPalette[paletteID][(r << 4) + c] = bIndexes[blue] | gIndexes[green] | rIndexes[red];
                }
            }
            else {
                Seek_Cur(&info, 0x10 * (3 * sizeof(byte)));
            }
        }

        CloseFile(&info);
    }
}
#endif

void SetPaletteFade(byte destPaletteID, byte srcPaletteA, byte srcPaletteB, ushort blendAmount, int startIndex, int endIndex)
{
    if (destPaletteID >= PALETTE_COUNT || srcPaletteA >= PALETTE_COUNT || srcPaletteB >= PALETTE_COUNT)
        return;

    if (blendAmount > 0xFF) {
        blendAmount = 0xFF;
    }

    if (startIndex >= endIndex)
        return;

    uint blendA         = 0xFF - blendAmount;
    ushort *dst         = &fullPalette[destPaletteID][startIndex];
    for (int l = startIndex; l <= endIndex; ++l) {
        uint clrA = GetPaletteEntry(srcPaletteA, l);
        uint clrB = GetPaletteEntry(srcPaletteB, l);
        int rA    = (clrA >> 16) & 0xFF;
        int rB    = (clrB >> 16) & 0xFF;
        int gA    = (clrA >> 8) & 0xFF;
        int gB    = (clrB >> 8) & 0xFF;
        int bA    = (clrA >> 0) & 0xFF;
        int bB    = (clrB >> 0) & 0xFF;
        
        *dst = RGB888_TO_RGB565((byte)((ushort)(rB * blendAmount + blendA * rA) >> 8),
                                (byte)((ushort)(gB * blendAmount + blendA * gA) >> 8),
                                (byte)((ushort)(bB * blendAmount + blendA * bA) >> 8));

        ++dst;
    }
}

#if RETRO_REV02
void BlendColours(byte paletteID, byte* coloursA, byte* coloursB, int alpha, int index, int count) {

    if (paletteID >= PALETTE_COUNT || !coloursA || !coloursB)
        return;

    if (alpha > 0xFF) {
        alpha = 0xFF;
    }
    else if (alpha < 0) {
        alpha = 0;
    }

    byte alpha2        = 0xFF - alpha;
    ushort *palettePtr = &fullPalette[paletteID][index];
    for (int i = index; i < index + count; ++i) {
        // bgrx formatted array
        int r = alpha * coloursB[2] + alpha2 * coloursA[2];
        int g = alpha * coloursB[1] + alpha2 * coloursA[1];
        int b = alpha * coloursB[0] + alpha2 * coloursA[0];
        coloursA += 4;
        coloursB += 4;
        *palettePtr = RGB888_TO_RGB565((byte)(r >> 8), (byte)(g >> 8), (byte)(b >> 8));
        ++palettePtr;
    }
}
#endif
