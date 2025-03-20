#include "voice.h"
#include <stdio.h>

#ifdef HAVE_ESPEAK
#include <espeak/speak_lib.h>
#endif

void voice_speak(const char *text) {
#ifdef HAVE_ESPEAK
    if (espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 500, NULL, 0) >= 0) {
        espeak_Synth(text, strlen(text) + 1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL);
        espeak_Synchronize();
    } else {
        printf("Voice feedback failed: %s\n", text);
    }
#else
    printf("Voice feedback unavailable: %s\n", text);
#endif
}