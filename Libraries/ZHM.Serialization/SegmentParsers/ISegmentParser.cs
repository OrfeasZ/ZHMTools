using ZHM.Common.IO;

namespace ZHM.Serialization.SegmentParsers
{
    interface ISegmentParser
    {
        object? ParseSegment(ZHMStream p_SegmentDataStream, ZHMStream p_ResourceDataStream);
    }
}
