using System;
using System.Runtime.InteropServices;

namespace Terran
{
	[StructLayout(LayoutKind.Sequential)]
	internal struct UUID
	{
		//public UUID(byte[] data) 
		//{
		//	m_Data = data;
		//}

		public static implicit operator byte[](UUID uuid) => uuid.ToArray();

		public override string ToString()
		{
			string result = "";
			int ind = 0;
			byte[] data = ToArray();
			for (int i = 0; i < 20; i++)
			{
				if (i == 4 || i == 7 || i == 10 || i == 13)
				{
					result += '-';
					continue;
				}

				result +=
							(data[ind] >> 4 & 0x0f).ToString("X") +
							(data[ind] & 0x0f).ToString("X");

				ind++;
			}

			return result;
		}

		public byte[] ToArray() 
		{
			unsafe 
			{
				fixed (byte* dataPtr = m_Data)
				{
					Span<byte> data = new Span<byte>(dataPtr, 16);
					return data.ToArray();
				}
			}
		}

        private unsafe fixed byte m_Data[16];
    }
}
