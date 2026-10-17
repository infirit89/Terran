using System;
using System.Runtime.InteropServices;

namespace Terran
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Color : IEquatable<Color>
    {
        public float R;
        public float G;
        public float B;
        public float A;

        public Color(float r, float g, float b)
        {
            R = r;
            G = g;
            B = b;
            A = 1.0f;
        }

        public Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public Color(int r, int g, int b) 
        {

        }

        public static Color White => new(1.0f, 1.0f, 1.0f, 1.0f);
        public static Color Black => new(0.0f, 0.0f, 0.0f, 1.0f);
        
        public static Color Red => new(1.0f, 0.0f, 0.0f, 1.0f);
        public static Color Green => new(0.0f, 1.0f, 0.0f, 1.0f);
        public static Color Blue => new(0.0f, 0.0f, 1.0f, 1.0f);
        
        public static Color Yellow => new(1.0f, 1.0f, 0.0f, 1.0f);
        public static Color Magenta => new(1.0f, 0.0f, 1.0f, 1.0f);
        public static Color Cyan => new(0.0f, 1.0f, 1.0f, 1.0f);

        public static Color Gray => new(0.5f, 0.5f, 0.5f, 1.0f);
        public static Color Add(Color a, Color b) => new(a.R + b.R, a.G + b.G, a.B + b.B, a.A + b.A);
        public static Color Subtract(Color a, Color b) => new(a.R - b.R, a.G - b.G, a.B - b.B, a.A - b.A);
        public static Color Multiply(Color a, Color b) => new(a.R * b.R, a.G * b.G, a.B * b.B, a.A * b.A);
        public static Color Multiply(Color a, float b) => new(a.R * b, a.G * b, a.B * b, a.A * b);
        public static Color Divide(Color a, Color b) => new(a.R / b.R, a.G / b.G, a.B / b.B, a.A / b.A);
        public static Color Divide(Color a, float b) => new(a.R / b, a.G / b, a.B / b, a.A / b);
        
        public static Color operator +(Color a, Color b) => Add(a, b);
        public static Color operator -(Color a, Color b) => Subtract(a, b);
        public static Color operator *(Color a, Color b) => Multiply(a, b);
        public static Color operator *(Color a, float b) => Multiply(a, b);
        public static Color operator /(Color a, Color b) => Divide(a, b);
        public static Color operator /(Color a, float b) => Divide(a, b);
        
        public static bool operator ==(Color a, Color b) =>
            ((a.R == b.R)) && ((a.G == b.G)) && ((a.B == b.B)) && ((a.A == b.A));
        public static bool operator !=(Color a, Color b) => !(a == b);

        public bool Equals(Color other)
        {
            return this == other;
        }

        public override bool Equals(object? obj)
        {
            if (obj == null || GetType() != obj.GetType())
                return false;
            
            return Equals((Color)obj);
        }

        public override string ToString() => $"R: {R}, G: {G}, B: {B}, A: {A}";

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = R.GetHashCode();
                hashCode = (hashCode * 397) ^ G.GetHashCode();
                hashCode = (hashCode * 397) ^ B.GetHashCode();
                hashCode = (hashCode * 397) ^ A.GetHashCode();
                return hashCode;
            }
        }
    }
}
