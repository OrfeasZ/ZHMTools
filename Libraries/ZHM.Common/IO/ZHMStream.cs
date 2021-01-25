using System;
using System.IO;
using System.Text;
using ZHM.Common.IO.Conversion;

namespace ZHM.Common.IO
{
    public class ZHMStream : Stream, IDisposable
    {
		/// <summary>
		/// Current position of the stream.
		/// </summary>
		public override long Position
		{
			get => BaseStream.Position;
			set => throw new NotSupportedException();
		}

		/// <summary>
		/// Length of the underlying stream.
		/// </summary>
		public override long Length => BaseStream.Length;

		/// <summary>
		/// The bit converter used to read values from the stream
		/// </summary>
		public EndianBitConverter BitConverter { get; }

		/// <summary>
		/// The endianness of the stream.
		/// </summary>
        public Endianness Endianness => BitConverter.Endianness;

		/// <summary>
		/// Gets the underlying stream.
		/// </summary>
		public Stream BaseStream { get; }

        // Declare our capabilities.
		public override bool CanRead => BaseStream.CanRead;
		public override bool CanSeek => BaseStream.CanSeek;
		public override bool CanWrite => BaseStream.CanWrite;

		/// <summary>
		/// Whether or not this reader has been disposed yet.
		/// </summary>
		protected bool m_Disposed;

		/// <summary>
		/// Buffer used for temporary storage before conversion into primitives
		/// </summary>
		protected readonly byte[] m_Buffer = new byte[16];

		/// <summary>
		/// Whether to dispose the base stream when disposing the stream.
		/// </summary>
		protected readonly bool m_ShouldDispose;

		/// <summary>
		/// A flipped bit converter used for reading and writing in a different endianness than the default.
		/// </summary>
        protected readonly EndianBitConverter m_FlippedConverter;

		public ZHMStream(Stream p_Stream, Endianness p_Endianness = Endianness.LittleEndian, bool p_ShouldDispose = true)
        {
            if (!p_Stream.CanWrite && !p_Stream.CanRead)
                throw new ArgumentException("Stream isn't writable or readable", nameof(p_Stream));

            m_ShouldDispose = p_ShouldDispose;
            BaseStream = p_Stream;

            BitConverter = p_Endianness == Endianness.LittleEndian ? (EndianBitConverter) EndianBitConverter.Little : EndianBitConverter.Big;
            m_FlippedConverter = p_Endianness == Endianness.LittleEndian ? (EndianBitConverter) EndianBitConverter.Big : EndianBitConverter.Little;
        }

		/// <summary>
		/// Reads a single byte from the stream.
		/// </summary>
		/// <returns>The byte read</returns>
		public override int ReadByte()
		{
			ReadInternal(m_Buffer, 0, 1);
			return m_Buffer[0];
		}

		/// <summary>
		/// Reads a single byte from the stream.
		/// </summary>
		/// <returns>The byte read</returns>
		public byte ReadUByte()
		{
			ReadInternal(m_Buffer, 0, 1);
			return m_Buffer[0];
		}

		/// <summary>
		/// Reads a single signed byte from the stream.
		/// </summary>
		/// <returns>The byte read</returns>
		public sbyte ReadSByte()
		{
			ReadInternal(m_Buffer, 0, 1);
			return unchecked((sbyte)m_Buffer[0]);
		}

		/// <summary>
		/// Reads a boolean from the stream. 1 byte is read.
		/// </summary>
		/// <returns>The boolean read</returns>
		public bool ReadBool()
		{
			ReadInternal(m_Buffer, 0, 1);
			return BitConverter.ToBoolean(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 16-bit signed integer from the stream, using the bit converter
		/// for this reader. 2 bytes are read.
		/// </summary>
		/// <returns>The 16-bit integer read</returns>
		public short ReadInt16()
		{
			ReadInternal(m_Buffer, 0, 2);
			return BitConverter.ToInt16(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 32-bit signed integer from the stream, using the bit converter
		/// for this reader. 4 bytes are read.
		/// </summary>
		/// <returns>The 32-bit integer read</returns>
		public int ReadInt32()
		{
			ReadInternal(m_Buffer, 0, 4);
			return BitConverter.ToInt32(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 64-bit signed integer from the stream, using the bit converter
		/// for this reader. 8 bytes are read.
		/// </summary>
		/// <returns>The 64-bit integer read</returns>
		public long ReadInt64()
		{
			ReadInternal(m_Buffer, 0, 8);
			return BitConverter.ToInt64(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 16-bit unsigned integer from the stream, using the bit converter
		/// for this reader. 2 bytes are read.
		/// </summary>
		/// <returns>The 16-bit unsigned integer read</returns>
		public ushort ReadUInt16()
		{
			ReadInternal(m_Buffer, 0, 2);
			return BitConverter.ToUInt16(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 32-bit unsigned integer from the stream, using the bit converter
		/// for this reader. 4 bytes are read.
		/// </summary>
		/// <returns>The 32-bit unsigned integer read</returns>
		public uint ReadUInt32()
		{
			ReadInternal(m_Buffer, 0, 4);
			return BitConverter.ToUInt32(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 64-bit unsigned integer from the stream, using the bit converter
		/// for this reader. 8 bytes are read.
		/// </summary>
		/// <returns>The 64-bit unsigned integer read</returns>
		public ulong ReadUInt64()
		{
			ReadInternal(m_Buffer, 0, 8);
			return BitConverter.ToUInt64(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a single-precision floating-point value from the stream, using the bit converter
		/// for this reader. 4 bytes are read.
		/// </summary>
		/// <returns>The floating point value read</returns>
		public float ReadSingle()
		{
			ReadInternal(m_Buffer, 0, 4);
			return BitConverter.ToSingle(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a double-precision floating-point value from the stream, using the bit converter
		/// for this reader. 8 bytes are read.
		/// </summary>
		/// <returns>The floating point value read</returns>
		public double ReadDouble()
		{
			ReadInternal(m_Buffer, 0, 8);
			return BitConverter.ToDouble(m_Buffer, 0);
		}

        /// <summary>
		/// Reads a 16-bit signed integer from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 2 bytes are read.
		/// </summary>
		/// <returns>The 16-bit integer read</returns>
		public short ReadInt16Flipped()
		{
			ReadInternal(m_Buffer, 0, 2);
			return m_FlippedConverter.ToInt16(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 32-bit signed integer from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 4 bytes are read.
		/// </summary>
		/// <returns>The 32-bit integer read</returns>
		public int ReadInt32Flipped()
		{
			ReadInternal(m_Buffer, 0, 4);
			return m_FlippedConverter.ToInt32(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 64-bit signed integer from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 8 bytes are read.
		/// </summary>
		/// <returns>The 64-bit integer read</returns>
		public long ReadInt64Flipped()
		{
			ReadInternal(m_Buffer, 0, 8);
			return m_FlippedConverter.ToInt64(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 16-bit unsigned integer from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 2 bytes are read.
		/// </summary>
		/// <returns>The 16-bit unsigned integer read</returns>
		public ushort ReadUInt16Flipped()
		{
			ReadInternal(m_Buffer, 0, 2);
			return m_FlippedConverter.ToUInt16(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 32-bit unsigned integer from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 4 bytes are read.
		/// </summary>
		/// <returns>The 32-bit unsigned integer read</returns>
		public uint ReadUInt32Flipped()
		{
			ReadInternal(m_Buffer, 0, 4);
			return m_FlippedConverter.ToUInt32(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a 64-bit unsigned integer from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 8 bytes are read.
		/// </summary>
		/// <returns>The 64-bit unsigned integer read</returns>
		public ulong ReadUInt64Flipped()
		{
			ReadInternal(m_Buffer, 0, 8);
			return m_FlippedConverter.ToUInt64(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a single-precision floating-point value from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 4 bytes are read.
		/// </summary>
		/// <returns>The floating point value read</returns>
		public float ReadSingleFlipped()
		{
			ReadInternal(m_Buffer, 0, 4);
			return m_FlippedConverter.ToSingle(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a double-precision floating-point value from the stream, using a flipped bit converter
		/// from the bit converter of this reader. 8 bytes are read.
		/// </summary>
		/// <returns>The floating point value read</returns>
		public double ReadDoubleFlipped()
		{
			ReadInternal(m_Buffer, 0, 8);
			return m_FlippedConverter.ToDouble(m_Buffer, 0);
		}

		/// <summary>
		/// Reads a string with a 32-bit length prefix.
		/// </summary>
		/// <returns></returns>
        public string ReadString()
        {
            var s_StringLength = ReadUInt32();
            return Encoding.UTF8.GetString(ReadBytes((int)s_StringLength));
        }

		/// <summary>
		/// Reads the specified number of bytes into the given buffer, starting at
		/// the given offset.
		/// </summary>
		/// <param name="p_Data">The buffer to copy data into</param>
		/// <param name="p_Offset">The offset to copy data into</param>
		/// <param name="p_Count"></param>
		/// <returns>The number of bytes actually read. This will only be less than
		/// the requested number of bytes if the end of the stream is reached.
		/// </returns>
		public virtual int ReadBytes(byte[] p_Data, int p_Offset, int p_Count)
		{
			CheckDisposed();

			if (p_Offset < 0)
				throw new ArgumentOutOfRangeException(nameof(p_Offset));

			if (p_Count < 0)
				throw new ArgumentOutOfRangeException(nameof(p_Count));

			if (p_Count + p_Offset > p_Data.Length)
				throw new ArgumentException("Not enough space in buffer for specified number of bytes starting at specified offset.");

			return ReadInternal(p_Data, p_Offset, p_Count);
		}

		/// <summary>
		/// Reads the specified number of bytes, returning them in a new byte array.
		/// If not enough bytes are available before the end of the stream, this
		/// method will throw an exception.
		/// </summary>
		/// <param name="p_Count">The number of bytes to read</param>
		/// <returns>The bytes read</returns>
		public virtual byte[] ReadBytes(int p_Count)
		{
			CheckDisposed();

			if (p_Count < 0)
				throw new ArgumentOutOfRangeException(nameof(p_Count));

			var s_Buffer = new byte[p_Count];
			var s_BytesRead = ReadInternal(s_Buffer, 0, p_Count);

			// If we didn't read the required amount of bytes then throw an exception.
			if (s_BytesRead != p_Count)
				throw new EndOfStreamException($"End of stream reached with {p_Count - s_BytesRead} byte{(p_Count - s_BytesRead == 1 ? "s" : "")} left to read.");

			return s_Buffer;
		}

		/// <summary>
		/// Disposes of the underlying stream.
		/// </summary>
		public new virtual void Dispose()
		{
			base.Dispose();

			CheckDisposed();

			m_Disposed = true;

			if (m_ShouldDispose)
				BaseStream.Dispose();
		}

		public override void Flush()
		{
			CheckDisposed();
			BaseStream.Flush();
		}

		public override long Seek(long p_Offset, SeekOrigin p_Origin)
		{
			CheckDisposed();
			return BaseStream.Seek(p_Offset, p_Origin);
		}

		public override int Read(byte[] p_Buffer, int p_Offset, int p_Count)
		{
			return ReadInternal(p_Buffer, p_Offset, p_Count);
		}

		public override void SetLength(long p_Value)
		{
			CheckDisposed();
			BaseStream.SetLength(p_Value);
		}

        /// <summary>
		/// Checks whether or not the reader has been disposed, throwing an exception if so.
		/// </summary>
		protected void CheckDisposed()
		{
			if (m_Disposed)
				throw new ObjectDisposedException(nameof(ZHMStream));
		}

		/// <summary>
		/// Read the specified number of bytes into the provided buffer, starting at the
		/// specified offset.
		/// </summary>
		/// <param name="p_Buffer">The buffer to read the data into</param>
		/// <param name="p_Offset">The offset to start writing the data at</param>
		/// <param name="p_Count">The number of bytes to read</param>
		/// <returns>The number of bytes read</returns>
		protected virtual int ReadInternal(byte[] p_Buffer, int p_Offset, int p_Count)
		{
			CheckDisposed();

			var s_BytesRead = BaseStream.Read(p_Buffer, p_Offset, p_Count);

            if (s_BytesRead != p_Count)
                throw new EndOfStreamException($"End of stream reached with {p_Count - s_BytesRead} byte{(p_Count - s_BytesRead == 1 ? "s" : "")} left to read.");

            return s_BytesRead;
        }

        public void Skip(long p_BytesToSkip)
        {
            if (p_BytesToSkip < 0)
                throw new ArgumentOutOfRangeException(nameof(p_BytesToSkip), "Bytes to skip must be a positive number.");

            Seek(p_BytesToSkip, SeekOrigin.Current);
        }

        public void AlignReadTo(int p_Alignment)
        {
            if (Position % p_Alignment == 0)
                return;

            var s_Number = p_Alignment - (Position % p_Alignment);
            Skip(s_Number);
		}

        /// <summary>
		/// Writes a boolean value to the stream. 1 byte is written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(bool p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 1);
		}

		/// <summary>
		/// Writes a 16-bit signed integer to the stream, using the bit converter
		/// for this writer. 2 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(short p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 2);
		}

		/// <summary>
		/// Writes a 32-bit signed integer to the stream, using the bit converter
		/// for this writer. 4 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(int p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 4);
		}

		/// <summary>
		/// Writes a 64-bit signed integer to the stream, using the bit converter
		/// for this writer. 8 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(long p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 8);
		}

		/// <summary>
		/// Writes a 16-bit unsigned integer to the stream, using the bit converter
		/// for this writer. 2 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(ushort p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 2);
		}

		/// <summary>
		/// Writes a 32-bit unsigned integer to the stream, using the bit converter
		/// for this writer. 4 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(uint p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 4);
		}

		/// <summary>
		/// Writes a 64-bit unsigned integer to the stream, using the bit converter
		/// for this writer. 8 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(ulong p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 8);
		}

		/// <summary>
		/// Writes a single-precision floating-point value to the stream, using the bit converter
		/// for this writer. 4 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(float p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 4);
		}

		/// <summary>
		/// Writes a double-precision floating-point value to the stream, using the bit converter
		/// for this writer. 8 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(double p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 8);
		}

		/// <summary>
		/// Writes a decimal value to the stream, using the bit converter for this writer.
		/// 16 bytes are written.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(decimal p_Value)
		{
			BitConverter.CopyBytes(p_Value, m_Buffer, 0);
			WriteInternal(m_Buffer, 0, 16);
		}

		/// <summary>
		/// Writes a signed byte to the stream.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(byte p_Value)
		{
			m_Buffer[0] = p_Value;
			WriteInternal(m_Buffer, 0, 1);
		}

		/// <summary>
		/// Writes an unsigned byte to the stream.
		/// </summary>
		/// <param name="p_Value">The value to write</param>
		public void Write(sbyte p_Value)
		{
			m_Buffer[0] = unchecked((byte)p_Value);
			WriteInternal(m_Buffer, 0, 1);
		}

		/// <summary>
		/// Writes an array of bytes to the stream.
		/// </summary>
		/// <param name="p_Value">The values to write</param>
		public void Write(byte[] p_Value)
		{
			WriteInternal(p_Value, 0, p_Value.Length);
		}

		/// <summary>
		/// Writes an length-prefixed string to the stream.
		/// </summary>
		/// <param name="p_Value">The string to write</param>
		public void Write(string p_Value)
        {
            var s_StringBytes = Encoding.UTF8.GetBytes(p_Value);
			Write((uint) (s_StringBytes.Length + 1)); // We add 1 for the null terminator.

			Write(s_StringBytes);
            Write((byte) 0x00); // Null terminator.
        }

        public void AlignWriteTo(int p_Alignment, byte p_PaddingByte)
        {
            if (Position % p_Alignment == 0)
                return;

            var s_Number = p_Alignment - (Position % p_Alignment);

            for (var i = 0; i < s_Number; ++i)
                Write(p_PaddingByte);
        }

		/// <summary>
		/// Writes a portion of an array of bytes to the stream.
		/// </summary>
		/// <param name="p_Value">An array containing the bytes to write</param>
		/// <param name="p_Offset">The index of the first byte to write within the array</param>
		/// <param name="p_Count">The number of bytes to write</param>
		public override void Write(byte[] p_Value, int p_Offset, int p_Count)
		{
			WriteInternal(p_Value, p_Offset, p_Count);
		}

        /// <summary>
		/// Writes the specified number of bytes from the start of the given byte array,
		/// after checking whether or not the writer has been disposed.
		/// </summary>
		/// <param name="p_Bytes">The array of bytes to write from</param>
		/// <param name="p_Offset">The offset to start reading data from the array</param>
		/// <param name="p_Length">The number of bytes to write</param>
		protected virtual void WriteInternal(byte[] p_Bytes, int p_Offset, int p_Length)
        {
            CheckDisposed();
            BaseStream.Write(p_Bytes, p_Offset, p_Length);
		}
	}
}
