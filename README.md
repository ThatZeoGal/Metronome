# Metronome
A wavmark (wmb) creator and reader for Rhythm Heaven Groove.

## What's a wavmark?
Wavmark is a small format used to store tempo data. It is made up of a series of "section markers", each of which holds the sample position of the marker and the amount of ticks between the marker and the next one (480 ticks = 1 beat).

The file also controls where the level chart begins playback. When the chosen section marker is reached, that's when chart playback begins. This can be used to start running game events later than the immediate start of the audio file.

Finally, there are some other values in the file. The first 4 bytes are always set to 0, as they are written to when loaded into memory. There is a field that holds the sample rate of the audio, but in my testing, it doesn't seem to do anything. There is a field that likely corresponds to the index of the section where looping starts, and there is also a field (either 0 or -1) that has no clear effect currently.

## Building
This project uses the Ninja build system. Simply run `ninja` in the project directory to build.

## Credits
- Wavmark research by [Zeo](https://github.com/ThatZeoGal), [Patata](https://github.com/patataofcourse), and [Conhlee](https://github.com/conhlee)
- Tool created by [Zeo](https://github.com/ThatZeoGal), with massive help from [Conhlee](https://github.com/conhlee)
