using System;
using System.IO;

namespace ZHM.Common.IO
{
    public class LimitedReader : ZHMStream
    {
        public override long Position => m_CurrentOffset;
        public override long Length => m_Limit;

        protected long m_Limit;
        protected long m_CurrentOffset;
        protected long m_StartOffset;

        public LimitedReader(ZHMStream p_Stream, long p_Limit, bool p_ShouldDispose = true) : 
            base(p_Stream, p_Stream.Endianness, p_ShouldDispose)
        {
            m_Limit = p_Limit;
            m_CurrentOffset = 0;
            m_StartOffset = p_Stream.Position;
        }

        public override long Seek(long p_Offset, SeekOrigin p_Origin)
        {
            CheckDisposed();

            // Find the requested target offset.
            var s_TargetOffset = p_Offset;

            if (p_Origin == SeekOrigin.End)
                s_TargetOffset = m_Limit - p_Offset;
            else if (p_Origin == SeekOrigin.Current)
                s_TargetOffset = m_CurrentOffset + p_Offset;

            if (s_TargetOffset < 0 || s_TargetOffset > m_Limit)
                throw new ArgumentException("The provided offset is out of bounds for this stream.", nameof(p_Offset));

            // Set the position.
            m_CurrentOffset = s_TargetOffset;

            // Seek internally.
            BaseStream.Seek(m_StartOffset + m_CurrentOffset, SeekOrigin.Begin);

            return m_CurrentOffset;
        }

        protected override int ReadInternal(byte[] p_Data, int p_Index, int p_Count)
        {
            CheckDisposed();

            // Check if we have enough bytes to read.
            var s_ToRead = p_Count;

            if (m_CurrentOffset + p_Count > m_Limit)
                s_ToRead = (int) m_Limit - (int) m_CurrentOffset;

            if (s_ToRead <= 0)
                return 0;

            // Read the data.
            var s_Read = BaseStream.Read(p_Data, p_Index, s_ToRead);
            m_CurrentOffset += s_Read;
            return s_Read;
        }
    }
}
