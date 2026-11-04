using System;
using System.Runtime.InteropServices;

namespace Terran
{
	// based on TMath's TVector3 class
	// https://github.com/OKAY808/TMath
	
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3 : IEquatable<Vector3>
	{
		public float X;
		public float Y;
		public float Z;

		public float Magnitude => (float)Math.Sqrt(X * X + Y * Y + Z * Z);

		public Vector3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		public Vector3(int x, int y, int z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		public Vector3(Vector2 vec, float z)
		{
			X = vec.X;
			Y = vec.Y;
			Z = z;
		}

		public Vector3(Vector2 vec) 
		{
			X = vec.X;
			Y = vec.Y;
			Z = 0.0f;
		}

		public Vector3 Normalized => new(X / Magnitude, Y / Magnitude, Z / Magnitude);

		public static Vector3 Zero => new(0.0f, 0.0f, 0.0f);
		public static Vector3 One => new(1.0f, 1.0f, 1.0f);

		public static Vector3 Right => new(1.0f, 0.0f, 0.0f);
		public static Vector3 Up => new(0.0f, 1.0f, 0.0f);
		public static Vector3 Forward => new(0.0f, 0.0f, 1.0f);

		public static Vector3 Negate(Vector3 vec) => new(-vec.X, -vec.Y, -vec.Z);
		
		public static Vector3 Add(Vector3 vec1, Vector3 vec2) => new(vec1.X + vec2.X, vec1.Y + vec2.Y, vec1.Z + vec2.Z);
		
		public static Vector3 Subtract(Vector3 vec1, Vector3 vec2) => new(vec1.X - vec2.X, vec1.Y - vec2.Y, vec1.Z - vec2.Z);

		public static Vector3 Multiply(Vector3 vec1, Vector3 vec2) => new(vec1.X * vec2.X, vec1.Y * vec2.Y, vec1.Z * vec2.Z);
		public static Vector3 Multiply(Vector3 vec1, float scalar) => new(vec1.X * scalar, vec1.Y * scalar, vec1.Z * scalar);
		public static Vector3 Multiply(Vector3 vec1, int scalar) => new(vec1.X * scalar, vec1.Y * scalar, vec1.Z * scalar);

		public static Vector3 Divide(Vector3 vec1, Vector3 vec2) => new(vec1.X / vec2.X, vec1.Y / vec2.Y, vec1.Z / vec2.Z);
		public static Vector3 Divide(Vector3 vec1, float scalar) => new(vec1.X / scalar, vec1.Y / scalar, vec1.Z / scalar);
		public static Vector3 Divide(Vector3 vec1, int scalar) => new(vec1.X / scalar, vec1.Y / scalar, vec1.Z / scalar);

		public static float Dot(in Vector3 a, in Vector3 b) => (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
		
		public static Vector3 Cross(in Vector3 a, in Vector3 b)
		{
            float x = a.Y * b.Z - b.Y * a.Z;
			float y = (a.X * b.Z - b.X * a.Z) * -1;
			float z = a.X * b.Y - b.X * a.Y;
			return new Vector3(x, y, z);
		}

		public static bool operator ==(Vector3 a, Vector3 b) => ((a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z));

		public static bool operator !=(Vector3 a, Vector3 b) => !(a == b);

		public static Vector3 operator +(Vector3 a) => a;
		public static Vector3 operator +(Vector3 a, Vector3 b) => Add(a, b);

		public static Vector3 operator -(Vector3 a) => Negate(a);
		public static Vector3 operator -(Vector3 a, Vector3 b) => Subtract(a, b);

		public static Vector3 operator *(Vector3 a, Vector3 b) => Multiply(a, b);
		public static Vector3 operator *(Vector3 a, int b) => Multiply(a, b);
		public static Vector3 operator *(Vector3 a, float b) => Multiply(a, b);
		public static Vector3 operator /(Vector3 a, float b) => Divide(a, b);
		public static Vector3 operator /(Vector3 a, int b) => Divide(a, b);
		public static Vector3 operator /(Vector3 a, Vector3 b) => Divide(a, b);

		public bool Equals(Vector3 other)
		{
			return this == other;
		}

		// override object.Equals
		public override bool Equals(object? obj)
		{
			if (obj == null || GetType() != obj.GetType())
				return false;

			return Equals((Vector3)obj);
		}


		// override object.GetHashCode
		public override int GetHashCode() => ((int)X ^ (int)Y ^ (int)Z);

		public override string ToString() => $"X: {X}, Y: {Y}, Z: {Z}";
	}
}