﻿using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Terran
{
	public enum RigidbodyType : byte 
	{
		Static = 0,
		Dynamic,
		Kinematic
	}

	public enum RigidbodySleepState : byte
	{
		Sleep = 0,
		Awake,
		NeverSleep
	}

	public enum ForceMode2D : byte
	{
		Force = 0,
		Impulse
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct RayCastHitInfo2D
	{
		public Vector2 Point;
		public Vector2 Normal;
		public Rigidbody2D Rigidbody;
		//public Collider2D Collider;

        public static implicit operator bool(RayCastHitInfo2D hit) => !hit.Equals(default(RayCastHitInfo2D));
	}

	public static class Physics2D 
	{
		public static RayCastHitInfo2D RayCast(Vector2 origin, Vector2 direction, float length = 10.0f, ushort layerMask = 0xFFFF)
		{
			RayCastHitInfo2D hitInfo;
			bool hasHit = Internal.Physics2D_RayCast(in origin, in direction, length, out hitInfo, layerMask);

            if(!hasHit)
                return default(RayCastHitInfo2D);

			return hitInfo;
		}

		public static RayCastHitInfo2D[] RayCastAll(Vector2 origin, Vector2 direction, float length = 10.0f, ushort layerMask = 0xFFFF) 
		{
			return Internal.Physics2D_RayCastAll(origin, direction, length, layerMask);
		}
	}
}
