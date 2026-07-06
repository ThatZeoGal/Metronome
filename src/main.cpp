#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "proc/WMBProc.hpp"
#include "proc/TempoChange.hpp"

#include <Macro.hpp>
#include <Error.hpp>
#include <File.hpp>

enum
{
    MODE_NONE,
    MODE_CREATE,
    MODE_READ
};

void printHelp(char **argv)
{
    printf("Metronome v1.1\n");
    printf("Metronome was built " __DATE__ " " __TIME__ "\n\n");

    printf("usage for create: %s create <path to wmb> <chart start index> <song length in beats> <beat,bpm>...\n", argv[0]);
    printf("                     <chart start index> (int): The chart will start when this tempo change is reached.\n");
    printf("                                                This is zero-indexed.\n");
    printf("                     <song length in beats> (float): Beats start at 0.\n");
    printf("                     <beat,bpm> (float, float): The beat of the tempo change, and the bpm to change to.\n");
    printf("                                                At least 1 pair is required. The first pair must be at beat 0.\n");
    printf("                                                Add more pairs to have more tempo sections.\n");
    printf("                                                Beats start at 0.\n");
    printf("usage for read: %s read <path to wmb>\n", argv[0]);
    exit(1);
}

void createWMB(char **argv, int argc)
{
    const char *pathToWMB = argv[2];
    s32 chartStartIndex = strtol(argv[3], NULL, 0);
    f32 songLengthBeats = strtof(argv[4], NULL);

    u32 tempoChangeCount = argc - 5;
    TempoChange *tempoChange = new TempoChange[tempoChangeCount];

    for (u32 i = 5; i < argc; i++)
    {
        const char *seqStart = argv[i];
        const char *commaPos = strchr(seqStart, ',');
        if (commaPos == NULL)
        {
            Panic("Beat/bpm pairs need a comma!");
        }
        tempoChange[i - 5].beat = strtof(seqStart, NULL);
        tempoChange[i - 5].bpm = strtof(commaPos + 1, NULL);
    }

    WMBFile wmbFile;
    wmbFile.setReserved(0);
    wmbFile.setSampleRate(48000);
    wmbFile.setUnk08(0);
    wmbFile.setChartStartIndex(chartStartIndex);
    wmbFile.setLoopStartIndex(0);
    wmbFile.setMarkerCount(tempoChangeCount + 1);

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

        f32 sampleCount = (60.0f / tempoChange[i].bpm) * wmbFile.getSampleRate() * sectionLengthBeats;
        f32 ticks = ((tempoChange[i].bpm * sampleCount) / 60.0f / wmbFile.getSampleRate()) * 480.0f;
        
        wmbFile.addMarker(i, nextPosSample, ticks);
        
        nextPosSample += sampleCount;
    }

    wmbFile.addMarker(wmbFile.getMarkerCount() - 1, nextPosSample, 0);

    delete[] tempoChange;

    Buffer wmbBuffer = wmbFile.build();

    if (!fileWriteData(pathToWMB, wmbBuffer))
    {
        Panic("main: failed to write WMB to path \"%s\"", pathToWMB);
    }
}

void readWMB(char **argv)
{
    const char *pathToWMB = argv[2];
    Buffer wmbBuffer = fileReadData(pathToWMB);

    WMBFile wmbFile(wmbBuffer);
    WMBSectionMarker *sectionMarkers = wmbFile.getSectionMarkers();

    u32 markerCount = wmbFile.getMarkerCount();
    u32 sampleRate = wmbFile.getSampleRate();

    printf("Sample Rate:            %d\n", sampleRate);
    printf("[unknown]:              0x%#x\n", wmbFile.getUnk08());
    printf("Chart Start Index:      %d\n", wmbFile.getChartStartIndex());
    printf("Loop Start Index:       %d\n", wmbFile.getLoopStartIndex());
    printf("# of Section Markers:   %d\n", markerCount);
    printf("=================================\n");

    f32 currentBeat = 0;
    for (u32 i = 0; i < markerCount - 1; i++)
    {
        u32 sampleDelta = sectionMarkers[i + 1].posSample - sectionMarkers[i].posSample;

        f32 beats = (float)sectionMarkers[i].lenTick / 480.0f;
        f32 minutes = ((float)sampleDelta / (float)sampleRate) / 60.0f;
        f32 bpm = beats / minutes;

        printf("Tempo Change %d:\n", i + 1);
        printf("    Beat: %f\n", currentBeat);
        printf("    BPM:  %f\n", bpm);

        currentBeat += beats;
    }
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printHelp(argv);
    }

    // get args
    s32 mode = MODE_NONE;
    const char *modeArg = argv[1];

    if (strcasecmp(modeArg, "create") == 0)
    {
        mode = MODE_CREATE;
    }
    else if (strcasecmp(modeArg, "read") == 0)
    {
        mode = MODE_READ;
    }
    else
    {
        Panic("Unknown mode: \"%s\"", modeArg);
    }

    if (mode == MODE_CREATE)
    {
        if (argc < 6)
        {
            Panic("Not enough arguments for create.");
        }
        createWMB(argv, argc);
    }
    else if (mode == MODE_READ)
    {
        readWMB(argv);
    }

    return 0;
}
