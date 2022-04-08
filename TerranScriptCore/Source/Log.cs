using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
	public class Log
	{
		enum LogLevel : byte
		{
			Trace   = 1 << 0,
			Warn    = 1 << 1,
			Error   = 1 << 2
		}

		public static void Trace(string message) => Internal.Log.Log_Internal((byte)LogLevel.Trace, message != null ? message : "");
		public static void Trace(string format, params object[] args) => Trace(string.Format(format, args));
		public static void Trace(bool message) => Trace(message.ToString());
		public static void Trace(int message) => Trace(message.ToString());
		public static void Trace(float message) => Trace(message.ToString());
		public static void Trace(double message) => Trace(message.ToString());
		public static void Trace(Vector3 message) => Trace(message.ToString());
		public static void Trace(object message) => Trace(message.ToString());

		public static void Warn(string message) => Internal.Log.Log_Internal((byte)LogLevel.Warn, message != null ? message : "");
		public static void Warn(string format, params object[] args) => Warn(string.Format(format, args));
		public static void Warn(int message) => Warn(message.ToString());
		public static void Warn(bool message) => Warn(message.ToString());
		public static void Warn(float message) => Warn(message.ToString());
		public static void Warn(double message) => Warn(message.ToString());
		public static void Warn(Vector3 message) => Warn(message.ToString());
		public static void Warn(object message) => Warn(message.ToString());

		public static void Error(string message) => Internal.Log.Log_Internal((byte)LogLevel.Error, message != null ? message : "");
		public static void Error(string format, params object[] args) => Error(string.Format(format, args));
		public static void Error(bool message) => Error(message.ToString());
		public static void Error(int message) => Error(message.ToString());
		public static void Error(float message) => Error(message.ToString());
		public static void Error(double message) => Error(message.ToString());
		public static void Error(Vector3 message) => Error(message.ToString());
		public static void Error(object message) => Error(message.ToString());
	}
}
