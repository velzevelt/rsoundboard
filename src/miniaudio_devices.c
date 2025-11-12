#define MINIAUDIO_IMPLEMENTATION
#include <external/miniaudio.h>
#include <stdio.h>

const char *get_backend_name(ma_backend backend)
{
    switch (backend)
    {
    case ma_backend_wasapi:
        return "WASAPI";
    case ma_backend_dsound:
        return "DirectSound";
    case ma_backend_winmm:
        return "WinMM";
    case ma_backend_coreaudio:
        return "Core Audio";
    case ma_backend_sndio:
        return "sndio";
    case ma_backend_audio4:
        return "audio4";
    case ma_backend_oss:
        return "OSS";
    case ma_backend_pulseaudio:
        return "PulseAudio";
    case ma_backend_alsa:
        return "ALSA";
    case ma_backend_jack:
        return "JACK";
    case ma_backend_aaudio:
        return "AAudio";
    case ma_backend_opensl:
        return "OpenSL";
    case ma_backend_webaudio:
        return "Web Audio";
    case ma_backend_null:
        return "Null";
    default:
        return "Unknown";
    }
}

int main(void)
{
    ma_context context;
    ma_device_info *pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    ma_result result;

    // Get available backends
    ma_backend backends[MA_BACKEND_COUNT];
    size_t backendCount;
    ma_get_enabled_backends(backends, MA_BACKEND_COUNT, &backendCount);

    printf("Available backends: ");
    for (size_t i = 0; i < backendCount; i++)
    {
        printf("%s ", get_backend_name(backends[i]));
    }
    printf("\n\n");

    // Initialize context with all available backends
    ma_context_config config = ma_context_config_init();
    result = ma_context_init(backends, backendCount, &config, &context);
    if (result != MA_SUCCESS)
    {
        printf("Failed to initialize context: %s\n", ma_result_description(result));
        return -1;
    }

    printf("Initialized backend: %s\n\n", get_backend_name(context.backend));

    // Get capture devices
    result = ma_context_get_devices(&context, NULL, NULL, &pCaptureDeviceInfos, &captureDeviceCount);
    if (result != MA_SUCCESS)
    {
        printf("Failed to retrieve device information: %s\n", ma_result_description(result));
        ma_context_uninit(&context);
        return -1;
    }

    printf("=== AUDIO INPUT DEVICES (%d found) ===\n\n", captureDeviceCount);

    if (captureDeviceCount == 0)
    {
        printf("No input devices found!\n");
    }
    else
    {
        for (ma_uint32 i = 0; i < captureDeviceCount; i++)
        {
            printf("Input Device %d:\n", i);
            printf("  Name: %s\n", pCaptureDeviceInfos[i].name);

            // Device ID (format depends on backend)
            // if (pCaptureDeviceInfos[i].id.type == ma_device_id_type_string) {
            //     printf("  ID: %s\n", pCaptureDeviceInfos[i].id.name);
            // }

            printf("  Is Default: %s\n", pCaptureDeviceInfos[i].isDefault ? "Yes" : "No");
            printf("\n");
        }
    }

    ma_context_uninit(&context);

    printf("Press Enter to exit...");
    getchar();

    return 0;
}