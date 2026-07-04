#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <Macro.hpp>
#include <Error.hpp>

struct TempoChange
{
    f32 beat;
    f32 bpm;
};

struct WavMarkSectionMarker
{
    u32 posSample;
    u32 lenTick;
};

struct WavMarkData
{
    u32 reserved;
    u32 sampleRate;
    u32 unk08;
    u32 chartStartIndex;
    u32 loopStartIndex;
};

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        printf("Metronome v1.0.1\n");
        printf("Metronome was built " __DATE__ " " __TIME__ "\n\n");

        printf("usage: %s <path to wmb> <chart start index> <song length in beats> <beat,bpm>...\n", argv[0]);
        printf("          <song length in beats> (float): Beats start at 0.\n");
        printf("          <chart start index> (int): The chart will start when this tempo change is reached.\n");
        printf("                                     This is zero-indexed.\n");
        printf("          <beat,bpm> (float, float): The beat of the tempo change, and the bpm to change to.\n");
        printf("                                     At least 1 pair is required. The first pair must be at beat 0.\n");
        printf("                                     Add more pairs to have more tempo sections.\n");
        printf("                                     Beats start at 0.\n");
        return 1;
    }

    // get args
    const char *pathToWMB = argv[1];
    f32 songLengthBeats = strtof(argv[2], NULL);
    s32 chartStartIndex = strtol(argv[3], NULL, 0);

    u32 tempoChangeCount = argc - 4;
    TempoChange *tempoChange = new TempoChange[tempoChangeCount];

    for (u32 i = 4; i < argc; i++)
    {
        const char *seqStart = argv[i];
        const char *commaPos = strchr(seqStart, ',');
        if (commaPos == NULL)
        {
            Panic("Beat/bpm pairs need a comma!");
        }
        tempoChange[i - 4].beat = strtof(seqStart, NULL);
        tempoChange[i - 4].bpm = strtof(commaPos + 1, NULL);
    }

    // make file
    FILE *fp = fopen(pathToWMB, "wb+");
    if (fp == NULL)
    {
        Panic("Invalid file path.");
    }

    WavMarkData header;
    header.reserved = 0;
    header.sampleRate = 48000;
    header.unk08 = 0;
    header.chartStartIndex = chartStartIndex;
    header.loopStartIndex = 0;

    fwrite(&header, sizeof(WavMarkData), 1, fp);

    WavMarkSectionMarker entry;
    u32 nextPosSample = 0;
    for (u32 i = 0; i < tempoChangeCount; i++)
    {
        f32 sectionLengthBeats = 0;

        if (i + 1 < tempoChangeCount)
        {
            sectionLengthBeats = tempoChange[i + 1].beat - tempoChange[i].beat;
        }
        else
        {
            sectionLengthBeats = songLengthBeats - tempoChange[i].beat;
        }

        f32 sampleCount = (60.0f / tempoChange[i].bpm) * header.sampleRate * sectionLengthBeats;
        f32 ticks = ((tempoChange[i].bpm * sampleCount) / 60.0f / header.sampleRate) * 480.0f;

        entry.posSample = nextPosSample;
        entry.lenTick = ticks;

        nextPosSample += sampleCount;

        fwrite(&entry, sizeof(WavMarkSectionMarker), 1, fp);
    }

    entry.posSample = nextPosSample;
    entry.lenTick = 0;
    fwrite(&entry, sizeof(WavMarkSectionMarker), 1, fp);

    fclose(fp);

    delete[] tempoChange;
    return 0;
}