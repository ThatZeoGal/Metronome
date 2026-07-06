#include "WMBProc.hpp"

#include <Error.hpp>

WMBFile::WMBFile(void) : mReserved(0), mSampleRate(48000), mUnk08(0), mChartStartIndex(0), mLoopStartIndex(0), mMarkerCount(0), mSectionMarkers(NULL)
{
    mReserved = 0;
}

WMBFile::WMBFile(const Buffer &data)
{
    if (!data.check())
    {
        Panic("WMBFile ctor: data buffer is invalid");
    }

    if (data.get_size() < sizeof(WMBHeader))
    {
        Panic("WMBFile ctor: data buffer is too small (0x%x < 0x%x).", data.get_size(), sizeof(WMBHeader));
    }

    const WMBHeader *header = data.data<const WMBHeader>();
    const WMBSectionMarker *sectionMarkers = data.data<const WMBSectionMarker>(sizeof(WMBHeader));

    mReserved = header->reserved;
    mSampleRate = header->sampleRate;
    mUnk08 = header->unk08;
    mChartStartIndex = header->chartStartIndex;
    mLoopStartIndex = header->loopStartIndex;

    u32 sectionMarkerDataSize = data.get_size() - sizeof(WMBHeader);
    mMarkerCount = sectionMarkerDataSize / sizeof(WMBSectionMarker);
    mSectionMarkers = new WMBSectionMarker[sectionMarkerDataSize];

    for (u32 i = 0; i < mMarkerCount; i++)
    {
        mSectionMarkers[i].posSample = sectionMarkers[i].posSample;
        mSectionMarkers[i].lenTick = sectionMarkers[i].lenTick;
    }
}

WMBFile::WMBFile(u32 reserved, u32 sampleRate, u32 unk08, u32 chartStartIndex, u32 loopStartIndex, u32 markerCount, const WMBSectionMarker *sectionMarkers) : mReserved(reserved), mSampleRate(sampleRate), mUnk08(unk08), mChartStartIndex(chartStartIndex), mLoopStartIndex(loopStartIndex), mMarkerCount(markerCount)
{
    u32 sectionMarkerDataSize = sizeof(WMBSectionMarker) * markerCount;
    mSectionMarkers = new WMBSectionMarker[sectionMarkerDataSize];

    memcpy(mSectionMarkers, sectionMarkers, sectionMarkerDataSize);
}

void WMBFile::setMarkerCount(u32 markerCount)
{
    mMarkerCount = markerCount;
    
    if (mSectionMarkers != NULL) {
        delete[] mSectionMarkers;
        mSectionMarkers = NULL;
    }

    mSectionMarkers = new WMBSectionMarker[sizeof(WMBSectionMarker) * markerCount];
}

void WMBFile::addMarker(u32 index, u32 samplePos, u32 ticks)
{
    WMBSectionMarker marker;
    mSectionMarkers[index].posSample = samplePos;
    mSectionMarkers[index].lenTick = ticks;
}

Buffer WMBFile::build(void)
{
    u32 sectionMarkersSize = sizeof(WMBSectionMarker) * mMarkerCount;

    u32 fileSize = sectionMarkersSize + sizeof(WMBHeader);

    Buffer buffer (fileSize);

    WMBHeader *header = buffer.data<WMBHeader>();
    WMBSectionMarker *sectionMarkers = buffer.data<WMBSectionMarker>(sizeof(WMBHeader));

    header->reserved = mReserved;
    header->sampleRate = mSampleRate;
    header->unk08 = mUnk08;
    header->chartStartIndex = mChartStartIndex;
    header->loopStartIndex = mLoopStartIndex;

    memcpy(sectionMarkers, mSectionMarkers, sectionMarkersSize);

    return buffer;
}