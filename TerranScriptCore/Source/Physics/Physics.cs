using System;

namespace TerranScriptCore
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

	public struct RayCastHitInfo2D
	{
		public Vector2 Point;
		public Vector2 Normal;
		public Rigidbody2D Rigidbody;
	}

	public class Physics2D 
	{
		public static bool RayCast(out RayCastHitInfo2D hitInfo, Vector2 origin, Vector2 direction, float length = 10.0f) 
		{
			Internal.RayCastHitInfo2D_Internal hitInfo_Internal;
			bool hasHit = Internal.Physics2D_RayCast_Internal(in origin, in direction, length, out hitInfo_Internal);

			hitInfo = new RayCastHitInfo2D();

			hitInfo.Point = hitInfo_Internal.Point;
			hitInfo.Normal = hitInfo_Internal.Normal;

			UUID id = new UUID(hitInfo_Internal.UUIDArray);
			Entity entity = Entity.FindWithID(id);

			if (entity != null) 
			{
				hitInfo.Rigidbody = entity.GetComponent<Rigidbody2D>();
			}

			return hasHit;
		}
	}
}
