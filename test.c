#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

	unsigned char *file_buffer = NULL;
	size_t file_size = 0;

	FILE *file = fopen(argc > 1 ? argv[1] : "../audio_lib/test_intro.flac", "rb");
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		rewind(file);

		file_buffer = malloc(file_size);

		if (file_buffer != NULL)
			fread(file_buffer, 1, file_size, file);

		fclose(file);
	}

	ClownAudio_SoundData *sound = ClownAudio_LoadSoundData(file_buffer, file_size, NULL, 0);

	if (sound)
		printf("Loaded sound\n");
	else
		printf("Couldn't load sound\n");
	fflush(stdout);

	ClownAudio_Sound instance = ClownAudio_CreateSound(sound, true);
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
	ClownAudio_DestroySound(instance);

	printf("Unloading sound\n");
	fflush(stdout);
	ClownAudio_UnloadSoundData(sound);

	printf("Deiniting mixer\n");
	fflush(stdout);
	ClownAudio_Deinit();

//	stb_leakcheck_dumpmem();

	return 0;
}
