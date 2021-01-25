using System;
using System.Collections.Generic;
using System.IO;
using ZHM.Common.IO;
using ZHM.Serialization.Models;

namespace ZHM.Serialization.SegmentParsers
{
    public class ZRuntimeResourceIDSegmentParser : ISegmentParser
    {
        public object? ParseSegment(ZHMStream p_SegmentDataStream, ZHMStream p_ResourceDataStream)
        {
            var s_ResourceIdCount = p_SegmentDataStream.ReadUInt32();

            Console.WriteLine($"Parsing {s_ResourceIdCount} resource ids.");

            var s_Ids = new List<ZRuntimeResourceID>();

            for (var i = 0; i < s_ResourceIdCount; ++i)
            {
                var s_Offset = p_SegmentDataStream.ReadInt32();

                // TODO: This is very very wrong. Fix it.
                p_ResourceDataStream.Seek(s_Offset, SeekOrigin.Begin);

                var s_ResourceId = new ZRuntimeResourceID()
                {
                    Id = p_ResourceDataStream.ReadUInt64(),
                };

                Console.WriteLine($"Read Resource ID '{s_ResourceId.Id}'.");

                s_Ids.Add(s_ResourceId);
            }

            return s_Ids;
        }
    }
}
