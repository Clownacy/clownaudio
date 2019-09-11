#include <stdbool.h>
#include <stdio.h>

//#define STB_LEAKCHECK_IMPLEMENTATION
//#include "stb_leakcheck.h"

#include "clownaudio/clownaudio.h"

int main(int argc, char *argv[])
{
	if (ClownAudio_Init())
		printf("Inited mixer\n");
	else
		printf("Couldn't init mixer\n");
	fflush(stdout);

	ClownAudio_Sound *sound = ClownAudio_LoadSound(argc > 1 ? argv[1] : "test.ogg", false);

	if (sound)
		printf("Loaded sound\n");
	else
		printf("Couldn't load sound\n");
	fflush(stdout);

	ClownAudio_SoundInstanceID instance = ClownAudio_PlaySound(sound, true);
	ClownAudio_UnpauseSound(instance);

	if (instance)
		printf("Started sound\n");
	else
		printf("Couldn't start sound\n");
	fflush(stdout);

	getchar();
	ClownAudio_FadeOutSound(instance, 5 * 1000);
	getchar();
	ClownAudio_FadeInSound(instance, 2 * 1000);
	getchar();

	printf("Stopping sound\n");
	fflush(stdout);
	ClownAudio_StopSound(instance);

	printf("Unloading sound\n");
	fflush(stdout);
	ClownAudio_UnloadSound(sound);

	printf("Deiniting mixer\n");
	fflush(stdout);
	ClownAudio_Deinit();

//	stb_leakcheck_dumpmem();

	return 0;
}
