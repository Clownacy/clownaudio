#include <stdbool.h>
#include <stdio.h>

//#define STB_LEAKCHECK_IMPLEMENTATION
//#include "stb_leakcheck.h"

#include "audio_lib/audio_lib.h"

int main(int argc, char *argv[])
{
	if (AudioLib_Init())
		printf("Inited mixer\n");
	else
		printf("Couldn't init mixer\n");
	fflush(stdout);

	AudioLib_Sound *sound = AudioLib_LoadSound(argc > 1 ? argv[1] : "test.ogg", false);

	if (sound)
		printf("Loaded sound\n");
	else
		printf("Couldn't load sound\n");
	fflush(stdout);

	AudioLib_SoundInstanceID instance = AudioLib_PlaySound(sound, true);
	AudioLib_UnpauseSound(instance);

	if (instance)
		printf("Started sound\n");
	else
		printf("Couldn't start sound\n");
	fflush(stdout);

	getchar();
	AudioLib_FadeOutSound(instance, 5 * 1000);
	getchar();
	AudioLib_FadeInSound(instance, 2 * 1000);
	getchar();

	printf("Stopping sound\n");
	fflush(stdout);
	AudioLib_StopSound(instance);

	printf("Unloading sound\n");
	fflush(stdout);
	AudioLib_UnloadSound(sound);

	printf("Deiniting mixer\n");
	fflush(stdout);
	AudioLib_Deinit();

//	stb_leakcheck_dumpmem();

	return 0;
}
