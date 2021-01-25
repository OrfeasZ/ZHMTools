using System;
using System.Collections.Generic;
using System.IO;
using ZHM.Common.IO;
using ZHM.Common.IO.Conversion;
using ZHM.Serialization.SegmentParsers;

namespace ZHM.Serialization
{
    public class BinaryDeserializer : IDisposable
    {
        public const uint BinaryResourceMagic = 0x314E4942; // BIN1

        public static bool IsBinaryResource(Stream p_Stream)
        {
            using var s_Reader = new ZHMStream(p_Stream, Endianness.LittleEndian, false);
            
            var s_HasValidMagic = s_Reader.ReadUInt32() == BinaryResourceMagic;
            
            s_Reader.Seek(-4, SeekOrigin.Current);
            
            return s_HasValidMagic;
        }

        private static readonly Dictionary<uint, ISegmentParser> SegmentParsers = new Dictionary<uint, ISegmentParser>()
        {
            { 0x12EBA5ED, new RebasingSegmentParser() },
            { 0x3989BF9F, new TypeIdSegmentParser() },
            { 0x578FBCEE, new ZRuntimeResourceIDSegmentParser() },
        };

        private ZHMStream m_Stream;

        public byte Alignment { get; private set; }

        public BinaryDeserializer(Stream p_Stream)
        {
            m_Stream = new ZHMStream(p_Stream, Endianness.LittleEndian, false);
        }

        public void Dispose()
        {
            m_Stream.Dispose();
        }

        public T Deserialize<T>() where T : new()
        {
            var s_Resource = new T();

            m_Stream.Skip(4); // Magic (BIN1)
            m_Stream.Skip(1); // Unknown (always 0?)

            Alignment = m_Stream.ReadUByte();
            var s_SegmentCount = m_Stream.ReadUByte();

            m_Stream.Skip(1); // Unknown (always 0?)

            var s_DataSize = m_Stream.ReadUInt32Flipped();

            m_Stream.Skip(4); // Unknown (maybe alignment?)

            var s_Data = m_Stream.ReadBytes((int) s_DataSize);
            using var s_DataReader = new ZHMStream(new MemoryStream(s_Data));

            for (var i = 0; i < s_SegmentCount; ++i)
            {
                var s_SegmentType = m_Stream.ReadUInt32();
                var s_SegmentSize = m_Stream.ReadUInt32();

                Console.WriteLine($"Found segment {i} with {s_SegmentSize} bytes of data. Type: {s_SegmentType:X08}.");

                if (SegmentParsers.TryGetValue(s_SegmentType, out var s_Parser))
                {
                    using var s_SegmentReader = new LimitedReader(m_Stream, s_SegmentSize, false);
                    s_Parser.ParseSegment(s_SegmentReader, s_DataReader);
                }
                else
                {
                    throw new Exception($"Could not find a segment parser for segment of type {s_SegmentType:X08}.");
                }
            }

            Console.WriteLine($"Read {s_SegmentCount} segments with {m_Stream.BaseStream.Length - m_Stream.BaseStream.Position} bytes left to read.");

            return s_Resource;
        }
    }
}
