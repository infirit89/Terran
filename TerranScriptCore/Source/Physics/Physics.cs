using System;

namespace TerranScriptCore
{
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

	public struct RayCastHitInfo2D
	{
		public Vector2 Point;
		public Vector2 Normal;
	}

	public class Physics2D 
	{
		public static RayCastHitInfo2D RayCast(Vector2 origin, Vector2 direction, float length = 10.0f) 
		{
			RayCastHitInfo2D hitInfo;
			Internal.Physics2D_RayCast_Internal(in origin, in direction, length, out hitInfo);
			return hitInfo;
		}
	}
}
