using System;
using System.Collections.Generic;
using System.IO;
using ZHM.Common.IO;
using ZHM.Serialization.Models;

namespace ZHM.Serialization.SegmentParsers
{
    public class TypeIdSegmentParser : ISegmentParser
    {
        public object? ParseSegment(ZHMStream p_SegmentDataStream, ZHMStream p_ResourceDataStream)
        {
            var s_TypeIndices = new List<uint>();

            var s_TypeCount = p_SegmentDataStream.ReadUInt32();

            for (var i = 0; i < s_TypeCount; ++i)
            {
                var s_TypeIndexOffset = p_SegmentDataStream.ReadInt32();

                p_ResourceDataStream.Seek(s_TypeIndexOffset, SeekOrigin.Begin);

                var s_TypeIndex = p_ResourceDataStream.ReadUInt32();
                s_TypeIndices.Add(s_TypeIndex);
            }

            var s_TypeIdCount = p_SegmentDataStream.ReadUInt32();

            Console.WriteLine($"Parsing {s_TypeIdCount} ids.");

            var s_TypeIds = new Dictionary<uint, TypeId>();

            for (var i = 0; i < s_TypeIdCount; ++i)
            {
                p_SegmentDataStream.AlignReadTo(4);

                var s_TypeId = new TypeId()
                {
                    Index = p_SegmentDataStream.ReadUInt32(),
                    Unk = p_SegmentDataStream.ReadInt32(),
                    Name = p_SegmentDataStream.ReadString(),
                };

                s_TypeIds.Add(s_TypeId.Index, s_TypeId);
            }

            var s_FinalTypes = new List<TypeId>();

            for (var i = 0; i < s_TypeIndices.Count; ++i)
            {
                var s_TypeIndex = s_TypeIndices[i];
                var s_TypeId = s_TypeIds[s_TypeIndex];

                Console.WriteLine($"[{i}] => {s_TypeId.Name}");

                s_FinalTypes.Add(s_TypeId);
            }

            return s_FinalTypes;
        }
    }
}
