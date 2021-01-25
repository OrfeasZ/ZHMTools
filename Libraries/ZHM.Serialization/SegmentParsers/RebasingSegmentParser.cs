using System.IO;
using ZHM.Common.IO;

namespace ZHM.Serialization.SegmentParsers
{
    public class RebasingSegmentParser : ISegmentParser
    {
        public object? ParseSegment(ZHMStream p_SegmentDataStream, ZHMStream p_ResourceDataStream)
        {
            var s_RelocationCount = p_SegmentDataStream.ReadUInt32();

            for (var i = 0; i < s_RelocationCount; ++i)
            {
                var s_RelocationOffset = p_SegmentDataStream.ReadUInt32();

                p_ResourceDataStream.Seek(s_RelocationOffset, SeekOrigin.Begin);
                var s_RelocValue = p_ResourceDataStream.ReadInt64();

                /*
                 * I don't think we need to emulate this relocation behavior right now
                 * but basically what happens is that if [s_RelocValue] is -1 then the
                 * value at [p_ResourceDataStream + s_RelocationOffset] is set to 0.
                 * If it's not, then it's set to a pointer pointing to
                 * [p_ResourceDataStream + s_RelocValue].
                 *
                 * For now in our implementation we'll just handle the -1 case.
                 */

                if (s_RelocValue == 0)
                {
                    p_ResourceDataStream.Seek(s_RelocationOffset, SeekOrigin.Begin);
                    p_ResourceDataStream.Write((ulong) 0);
                }
            }

            return null;
        }
    }
}
