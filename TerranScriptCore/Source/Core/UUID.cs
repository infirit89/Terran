using System;

namespace Terran
{
	public class UUID
	{
		public UUID() 
		{
			m_Data = new byte[16];
		}

		public UUID(byte[] data) 
		{
			m_Data = data;
		}

		public static implicit operator byte[](UUID uuid) => uuid.Data;

		public override string ToString()
		{
			string result = "";
			int ind = 0;
			for (int i = 0; i < 20; i++)
			{
				if (i == 4 || i == 7 || i == 10 || i == 13)
				{
					result += '-';
					continue;
				}

				result +=
							(m_Data[ind] >> 4 & 0x0f).ToString("X") +
							(m_Data[ind] & 0x0f).ToString("X");

				ind++;
			}

			return result;
		}

		public byte[] Data 
		{
			get => m_Data;
		}

		private byte[] m_Data;
	}
}
