#ifndef PROC_WMB_PROC_HPP
#define PROC_WMB_PROC_HPP

#include <Type.hpp>

#include <Buffer.hpp>

#include <Macro.hpp>

typedef struct __attribute__((packed))
{
    u32 reserved;
    u32 sampleRate;
    u32 unk08;
    u32 chartStartIndex;
    u32 loopStartIndex;
} WMBHeader;
STRUCT_SIZE_ASSERT(WMBHeader, 0x14);

typedef struct __attribute__((packed))
{
    u32 posSample;
    u32 lenTick;
} WMBSectionMarker;
STRUCT_SIZE_ASSERT(WMBSectionMarker, 0x8);

class WMBFile
{
public:
    WMBFile(void);
    WMBFile(const Buffer &data);
    WMBFile(u32 reserved, u32 sampleRate, u32 unk08, u32 chartStartIndex, u32 loopStartIndex, u32 markerCount, const WMBSectionMarker *sectionMarkers);

    u32 getReserved(void) const { return mReserved; }
    void setReserved(u32 reserved) { mReserved = reserved; }

    u32 getSampleRate(void) const { return mSampleRate; }
    void setSampleRate(u32 sampleRate) { mSampleRate = sampleRate; }

    u32 getUnk08(void) const { return mUnk08; }
    void setUnk08(u32 unk08) { mUnk08 = unk08; }

    u32 getChartStartIndex(void) const { return mChartStartIndex; }
    void setChartStartIndex(u32 chartStartIndex) { mChartStartIndex = chartStartIndex; }

    u32 getLoopStartIndex(void) const { return mLoopStartIndex; }
    void setLoopStartIndex(u32 loopStartIndex) { mLoopStartIndex = loopStartIndex; }

    const WMBSectionMarker *getSectionMarkers(void) const { return mSectionMarkers; }
    WMBSectionMarker *getSectionMarkers(void) { return mSectionMarkers; }

    u32 getMarkerCount(void) const { return mMarkerCount; }
    void setMarkerCount(u32 markerCount);

    void addMarker(u32 index, u32 samplePos, u32 ticks);

    Buffer build(void);

private:
    u32 mReserved;
    u32 mSampleRate;
    u32 mUnk08;
    u32 mChartStartIndex;
    u32 mLoopStartIndex;
    WMBSectionMarker *mSectionMarkers;

    u32 mMarkerCount;
};

#endif