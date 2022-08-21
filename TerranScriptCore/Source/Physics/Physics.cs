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

	public struct RayCastHitInfo2D
	{
		public Vector2 Point;
		public Vector2 Normal;
		public Rigidbody2D Rigidbody;
		public Collider2D Collider;

        public static implicit operator bool(RayCastHitInfo2D hit) => !hit.Equals(default(RayCastHitInfo2D));
	}

	public class Physics2D 
	{
		public static RayCastHitInfo2D RayCast(Vector2 origin, Vector2 direction, float length = 10.0f, ushort layerMask = 0xFFFF)
		{
			Internal.RayCastHitInfo2D_Internal hitInfo_Internal;
			bool hasHit = Internal.Physics2D_RayCast(in origin, in direction, length, out hitInfo_Internal, layerMask);

            if(!hasHit)
                return default(RayCastHitInfo2D);

			RayCastHitInfo2D hitInfo = new RayCastHitInfo2D();

			hitInfo.Point = hitInfo_Internal.Point;
			hitInfo.Normal = hitInfo_Internal.Normal;

			UUID id = new UUID(hitInfo_Internal.UUIDArray);
			Entity entity = Entity.FindWithID(id);

			if (entity != null) 
			{
				hitInfo.Rigidbody = entity.GetComponent<Rigidbody2D>();
				hitInfo.Collider = entity.GetComponent<Collider2D>();
			}

			return hitInfo;
		}
	}
}
