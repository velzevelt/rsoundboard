#pragma once
#include <raylib.h>

Shader InitSDFShader();
Shader GetSDFShader();
Font LoadFontSDF(const char *fontName, int fontSize);
Font LoadFontSDFFromMemory(unsigned char *fileData, int fileSize, int fontSize);