#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

//#define STB_LEAKCHECK_IMPLEMENTATION
//#include "stb_leakcheck.h"

#include "clownaudio/clownaudio.h"

static void FileToMemory(const char *filename, unsigned char **buffer, size_t *size)
{
	*buffer = NULL;
	*size = 0;

	FILE *file = fopen(filename, "rb");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		size_t _size = ftell(file);
		rewind(file);

		unsigned char *_buffer = malloc(_size);

		if (_buffer != NULL)
		{
			fread(_buffer, 1, _size, file);
			*buffer = _buffer;
			*size = _size;
		}

		fclose(file);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3)
	{
		printf("clownaudio test program\n\nUsage: %s [intro file] [loop file (optional)]\n\n", argv[0]);
		return 0;
	}

	if (ClownAudio_Init())
	{
		printf("Inited mixer\n");
		fflush(stdout);

		unsigned char *file_buffers[2];
		size_t file_sizes[2];
		if (argc == 3)
		{
			FileToMemory(argv[1], &file_buffers[0], &file_sizes[0]);
			FileToMemory(argv[2], &file_buffers[1], &file_sizes[1]);
		}
		else if (argc == 2)
		{
			FileToMemory(argv[1], &file_buffers[0], &file_sizes[0]);
			file_buffers[1] = NULL;
			file_sizes[1] = 0;
		}

		ClownAudio_SoundData *sound = ClownAudio_LoadSoundData(file_buffers[0], file_sizes[0], file_buffers[1], file_sizes[1], false);

		if (sound != NULL)
		{
			printf("Loaded sound\n");
			fflush(stdout);

			ClownAudio_Sound instance = ClownAudio_CreateSound(sound, true, true);
			ClownAudio_UnpauseSound(instance);

			if (instance != 0)
			{
				printf("Started sound\n");
				fflush(stdout);

				float pan = 0.0f;
				bool pause = false;

				bool exit = false;
				while (!exit)
				{
					char input = getchar();

					switch (input)
					{
						case 'q':
							exit = true;
							break;

						case 'r':
							ClownAudio_RewindSound(instance);
							break;

						case 'o':
							ClownAudio_FadeOutSound(instance, 2 * 1000);
							break;

						case 'i':
							ClownAudio_FadeInSound(instance, 2 * 1000);
							break;

						case 'u':
							ClownAudio_SetSoundSampleRate(instance, 8000, 8000);
							break;

						case 'p':
							if (pause)
								ClownAudio_UnpauseSound(instance);
							else
								ClownAudio_PauseSound(instance);

							pause = !pause;

							break;

						case '[':
							pan -= 0.25f;
							ClownAudio_SetSoundPan(instance, pan);
							break;

						case ']':
							pan += 0.25f;
							ClownAudio_SetSoundPan(instance, pan);
							break;
					}
				}

				printf("Stopping sound\n");
				fflush(stdout);
				ClownAudio_DestroySound(instance);
			}
			else
			{
				printf("Couldn't start sound\n");
			}

			printf("Unloading sound\n");
			fflush(stdout);
			ClownAudio_UnloadSoundData(sound);

			free(file_buffers[1]);
			free(file_buffers[0]);
		}
		else
		{
			printf("Couldn't load sound\n");
		}

		printf("Deiniting mixer\n");
		fflush(stdout);
		ClownAudio_Deinit();
	}
	else
	{
		printf("Couldn't init mixer\n");
	}


//	stb_leakcheck_dumpmem();

	return 0;
}
