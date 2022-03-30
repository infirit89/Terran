using System;
using System.Runtime.InteropServices;

namespace TerranScriptCore
{
	// based on TMath's TVector2 class
	// https://github.com/OKAY808/TMath

	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2 : IEquatable<Vector2>
	{
		public float X;
		public float Y;

		public double Magnitude
		{
			get => Math.Sqrt(X * X + Y * Y);
		}

		public Vector2(float x, float y)
		{
			X = x;
			Y = y;
		}

		public Vector2(int x, int y)
		{

			X = x;
			Y = y;
		}

		public Vector2(double x, double y)
		{
			X = (float)x;
			Y = (float)y;
		}

		public static Vector2 Zero = new Vector2(0, 0);
		public static Vector2 One = new Vector2(1, 1);

		public static Vector2 Up = new Vector2(0, 1);
		public static Vector2 Down = new Vector2(0, -1);
		public static Vector2 Left = new Vector2(-1, 0);
		public static Vector2 Right = new Vector2(1, 0);
		public static double Dot(in Vector2 a, in Vector2 b) => (a.X * b.X) + (a.Y * b.Y);
		public void Set(int x, int y) { X = x; Y = y; }

		public void Set(float x, float y) { X = x; Y = y; }

		public void Set(double x, double y) { X = (float)x; Y = (float)y; }

		public static Vector2 Add(Vector2 vec1, Vector2 vec2) => new Vector2(vec1.X + vec2.X, vec1.Y + vec2.Y);
		public static Vector2 Subtract(Vector2 vec1, Vector2 vec2) => new Vector2(vec1.X - vec2.X, vec1.Y - vec2.Y);

		public static Vector2 Multiply(Vector2 vec1, Vector2 vec2) => new Vector2(vec1.X * vec2.X, vec1.Y * vec2.Y);
		public static Vector2 Multiply(Vector2 vec1, double scalar) => new Vector2(vec1.X * scalar, vec1.Y * scalar);
		public static Vector2 Multiply(Vector2 vec1, float scalar) => new Vector2(vec1.X * scalar, vec1.Y * scalar);
		public static Vector2 Multiply(Vector2 vec1, int scalar) => new Vector2(vec1.X * scalar, vec1.Y * scalar);

		public static Vector2 Divide(Vector2 vec1, Vector2 vec2) => new Vector2(vec1.X / vec2.X, vec1.Y / vec2.Y);
		public static Vector2 Divide(Vector2 vec1, double scalar) => new Vector2(vec1.X / scalar, vec1.Y / scalar);
		public static Vector2 Divide(Vector2 vec1, float scalar) => new Vector2(vec1.X / scalar, vec1.Y / scalar);
		public static Vector2 Divide(Vector2 vec1, int scalar) => new Vector2(vec1.X / scalar, vec1.Y / scalar);


		public static bool operator ==(Vector2 a, Vector2 b) => ((a.X == b.X) && (a.Y == b.Y));

		public static bool operator !=(Vector2 a, Vector2 b) => !(a == b);

		public static Vector2 operator +(Vector2 a) => a;
		public static Vector2 operator +(Vector2 a, Vector2 b) => Add(a, b);

		public static Vector2 operator -(Vector2 a) => new Vector2(-a.X, -a.Y);
		public static Vector2 operator -(Vector2 a, Vector2 b) => Subtract(a, b);

		public static Vector2 operator *(Vector2 a, Vector2 b) => Multiply(a, b);
		public static Vector2 operator *(Vector2 a, double b) => Multiply(a, b);
		public static Vector2 operator *(Vector2 a, float b) => Multiply(a, b);
		public static Vector2 operator *(Vector2 a, int b) => Multiply(a, b);

		public static Vector2 operator /(Vector2 a, double b) => Divide(a, b);
		public static Vector2 operator /(Vector2 a, float b) => Divide(a, b);
		public static Vector2 operator /(Vector2 a, int b) => Divide(a, b);
		public static Vector2 operator /(Vector2 a, Vector2 b) => Divide(a, b);


		public bool Equals(Vector2 other)
		{
			if (other == null)
				return false;

			return this == other;
		}

		// override object.Equals
		public override bool Equals(object obj)
		{
			if (obj == null || GetType() != obj.GetType())
				return false;

			return Equals((Vector2)obj);
		}


		// override object.GetHashCode
		public override int GetHashCode() => ((int)X ^ (int)Y);

		public override string ToString() => string.Format("X: {0}, Y: {1}", X, Y);
	}
}
