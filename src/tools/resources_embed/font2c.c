#include <raylib.h>
#include <stdio.h>

void PrintUsageMessage(const char *programName)
{
    printf("USAGE: %s input_font_path.ttf output_path.h\n", programName);
}

int main(int argc, char **argv)
{
    // printf("%s\n", argv[0]);

    // Have to create a window, cannot create opengl context, therefore use raylib functions
    SetConfigFlags(FLAG_WINDOW_MINIMIZED | FLAG_WINDOW_UNDECORATED);
    InitWindow(1, 1, "font2c dummy");

    if (argc == 1)
    {
        PrintUsageMessage(argv[0]);
        return 1;
    }

    if (argc < 3)
    {
        printf("Error: expected 2 arguments\n");
        PrintUsageMessage(argv[0]);
        return 1;
    }

    const char *outputPath = argv[2];
    const char *outputFileName = GetFileName(outputPath);

    if (!IsFileExtension(outputFileName, ".h"))
    {
        printf("Error: output filename should be c header extension (.h) %s\n", outputFileName);
        return 1;
    }

    const char *fontPath = argv[1];
    if (!FileExists(fontPath))
    {
        printf("Error: expected font %s does not exist\n", fontPath);
        return 1;
    }

    Font font = LoadFont(fontPath);

    // Font font = LoadFontSDF(fontPath);
    if (!IsFontValid(font))
    {
        printf("Error: invalid font %s\n", fontPath);
        return 1;
    }

    if (!ExportFontAsCode(font, outputPath))
    {
        printf("Error: generator failed %s\n", fontPath);
        return 1;
    }

    printf("Success: Generated file %s", outputPath);
    return 0;
}