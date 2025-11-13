#include <raylib.h>
#include <stdio.h>

void PrintUsageMessage(const char *programName)
{
    printf("USAGE: %s input_file_path output_file_path.h\n", programName);
}

int main(int argc, char **argv)
{
    // printf("%s\n", argv[0]);

    // Creates opengl context this way
    InitWindow(1, 1, "dummy");

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

    const char *filePath = argv[1];
    if (!FileExists(filePath))
    {
        printf("Error: expected file %s does not exist\n", filePath);
        return 1;
    }

    Image img = LoadImage(filePath);

    if (!IsImageValid(img))
    {
        printf("Error: invalid image %s\n", filePath);
        return 1; 
    }

    if (!ExportImageAsCode(img, outputPath))
    {
        printf("Error: generator failed %s\n", filePath);
        return 1;
    }

    printf("Success: Generated file %s", outputPath);
    return 0;
}