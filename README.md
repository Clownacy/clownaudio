## About

clownaudio is a sound engine, capable of playing and mixing sounds in a variety
of formats.

Supported formats include...
* Ogg Vorbis
* FLAC
* WAV
* Opus
* Various tracker formats ('.it', '.xm', '.mod', etc.)
* PxTone Music
* PxTone Noise
* SNES SPC

clownaudio is a 'full stack' sound engine, meaning that it handles everything
from decoding to playback - the user merely has to provide it with sound data to
process.

That said, clownaudio's internals are modular, meaning you can easily extract
its mixer and use it as part of your own audio system.


## Licensing

clownaudio itself is under the zlib licence.

However, be aware that clownaudio leverages other open-source libraries, which
are subject to their own licences.
