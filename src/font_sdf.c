#include "font_sdf.h"

// Font LoadFontSDF(const char *fontName, int fontSize, int *codepoints, int codepointsSize)
// {
//     int fileSize = 0;
//     unsigned char *fileData = LoadFileData(fontName, &fileSize);

//     // SDF font generation from TTF font
//     Font fontSDF = {0};
//     fontSDF.baseSize = fontSize;
//     fontSDF.glyphCount = codepointsSize;
//     fontSDF.glyphs = LoadFontData(fileData, fileSize, fontSize, codepoints, codepointsSize, FONT_SDF);

//     Image atlas = GenImageFontAtlas(fontSDF.glyphs, &fontSDF.recs, codepointsSize, fontSize, 0, 1);
//     fontSDF.texture = LoadTextureFromImage(atlas);
//     // SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_BILINEAR); // Required for SDF font
//     SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_TRILINEAR); // Required for SDF font
//     UnloadImage(atlas);

//     UnloadFileData(fileData);

//     return fontSDF;
// }

Font LoadFontSDFFromMemory(unsigned char *fileData, int fileSize, int fontSize)
{
    // SDF font generation from TTF font
    Font fontSDF = {0};
    fontSDF.baseSize = fontSize;
    fontSDF.glyphCount = 95;
    fontSDF.glyphs = LoadFontData(fileData, fileSize, fontSize, 0, fontSDF.glyphCount, FONT_SDF);

    Image atlas = GenImageFontAtlas(fontSDF.glyphs, &fontSDF.recs, fontSDF.glyphCount, fontSize, 0, 1);
    fontSDF.texture = LoadTextureFromImage(atlas);
    // SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_BILINEAR); // Required for SDF font
    SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_TRILINEAR); // Required for SDF font
    UnloadImage(atlas);

    return fontSDF;
}

Font LoadFontSDF(const char *fontName, int fontSize)
{
    int fileSize = 0;
    unsigned char *fileData = LoadFileData(fontName, &fileSize);
    Font fontSDF = LoadFontSDFFromMemory(fileData, fileSize, fontSize);
    UnloadFileData(fileData);
    return fontSDF;
}

Shader GetSDFShader()
{
    static Shader SDF;
    static bool SDFLoadDone;

    if (!SDFLoadDone)
    {
        SDF = LoadShader(0, "sdf_text.fs");
        SDFLoadDone = true;
    }

    return SDF;
}