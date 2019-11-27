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

	unsigned char *file_buffer2 = NULL;
	size_t file_size2 = 0;

	file = fopen(argc > 2 ? argv[2] : "../audio_lib/test_loop.flac", "rb");
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		file_size2 = ftell(file);
		rewind(file);

		file_buffer2 = malloc(file_size2);

		if (file_buffer2 != NULL)
			fread(file_buffer2, 1, file_size2, file);

		fclose(file);
	}

	ClownAudio_SoundData *sound = ClownAudio_LoadSoundData(file_buffer, file_size, file_buffer2, file_size2);

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
	ClownAudio_SetSoundSampleRate(instance, 8000);
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
