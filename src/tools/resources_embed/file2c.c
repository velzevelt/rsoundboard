#include <raylib.h>
#include <stdio.h>
#include <font_sdf.h>

void PrintUsageMessage(const char *programName)
{
    printf("USAGE: %s input_font_path output_path.h\n", programName);
}

int main(int argc, char **argv)
{
    // printf("%s\n", argv[0]);

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

    unsigned char *fileRawData = 0;
    int fileDataSize = 0;
    fileRawData = LoadFileData(filePath, &fileDataSize);

    if (!ExportDataAsCode(fileRawData, fileDataSize, outputPath))
    {
        printf("Error: generator failed %s\n", filePath);
        return 1;
    }

    printf("Success: Generated file %s", outputPath);
    return 0;
}