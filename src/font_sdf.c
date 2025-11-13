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
        const char *fragmentShaderSource =
            "#version 330 core\n"
            "in vec2 fragTexCoord;\n"
            "in vec4 fragColor;\n"
            "uniform sampler2D texture0;\n"
            "const float smoothing = 1.0 / 16.0;\n"
            "uniform float outlineWidth;\n"
            "uniform vec4 outlineColor;\n"
            "out vec4 finalColor;\n"
            "void main()\n"
            "{\n"
            "    float outerEdgeCenter = 0.5 - outlineWidth;\n"
            "    float distance = texture(texture0, fragTexCoord).a;\n"
            "    float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, distance);\n"
            "    float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);\n"
            "    vec4 color = vec4(\n"
            "        mix(outlineColor.rgb, fragColor.rgb, border), \n"
            "        mix(outlineColor.a, fragColor.a, border) * alpha\n"
            "    );\n"
            "    finalColor = color;\n"
            "}\n";

        // SDF = LoadShader(0, "sdf_text.fs");
        SDF = LoadShaderFromMemory(0, fragmentShaderSource);
        SDFLoadDone = true;
    }

    return SDF;
}