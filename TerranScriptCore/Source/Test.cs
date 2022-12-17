using System;
using System.Runtime.ConstrainedExecution;

namespace Terran
{
	public class TestScript : Scriptable
	{
		enum Test 
		{
			A = 0,
			B,
			C
		}

		public bool bTest = false; 
		public char cTest = 'A';
		
		public sbyte I8Test = 126;
		public short I16Test = -1210;
		public int I32Test = -15;
		public long I64Test = -100000000123123;
		
		public byte U8Test = 126;
		public ushort U16Test = 12023;
		public uint U32Test = 1231201231;
		public ulong U64Test = 12389056893832234879;

		public float FTest = 3213.3123123f;
		public double DTest = 3213.312312312312312;

		public string StrTest = "Hello World!";

		public Vector2 Vec2Test = new Vector2(10.0f, 1231.123f);
		public Vector3 Vec3Test = new Vector3(12312.0f, 1223.0f, 131.01f);
		
		private CircleRenderer _CircleRenderer;
		protected override void Init()
		{
			_CircleRenderer = Entity.GetComponent<CircleRenderer>();
			Log.Trace(_CircleRenderer.Thickness);
			Log.Trace(_CircleRenderer.Color);
		}

		protected override void Update(float deltaTime) 
		{
			if (Input.IsKeyPressed(KeyCode.Q))
				_CircleRenderer.Color = Color.Cyan;

			if (Input.IsKeyPressed(KeyCode.Up))
				_CircleRenderer.Thickness++;
			else if (Input.IsKeyPressed(KeyCode.Down))
				_CircleRenderer.Thickness--;
		}

		protected override void OnCollisionBegin(Entity entity) 
		{
			Log.Trace("collided");
		}
	}

	// Test script
	public class TestScriptable : Scriptable
	{
		Rigidbody2D rb;
		public float JumpForce = 100.0f;
		private bool m_CanJump = true;

		private bool m_HasCollided = false;

		// Runs when the entity in the current scene, that has this script, is started
		protected override void Init() 
		{
			
			//RayCastHitInfo2D hitInfo = Physics2D.RayCast(entity.transform.Position, -Vector2.Up);
			rb = Entity.GetComponent<Rigidbody2D>();	
			Log.Trace(JumpForce);
		}

		protected override void Update(float deltaTime)
		{
			// RayCastHitInfo2D hitInfo;
			// float rayLength = (entity.transform.Scale.Y * 0.5f) + 0.2f;
			// bool hasHit = Physics2D.RayCast(out hitInfo, entity.transform.Position, -Vector2.Up, rayLength);
			//
			// m_CanJump = hasHit;
			//
			// if (hasHit && !m_HasCollided)
			// {
			// 	Log.Trace(hitInfo.Rigidbody.entity.Name);
			// 	Log.Trace("ray hit");
			// }
			//
			// // Code to move the entity up and down
			// if (Input.IsKeyPressed(KeyCode.D))
			// 	// if w is pressed move the entity up one unit
			// 	entity.transform.Position += new Vector3(0.1f, 0.0f, 0.0f);
			// else if (Input.IsKeyPressed(KeyCode.A))
			// 	// if s is pressed move the entity down one unit
			// 	entity.transform.Position -= new Vector3(0.1f, 0.0f, 0.0f);
			//
			// if (Input.IsKeyPressed(KeyCode.Space) && m_CanJump) 
			// {
			// 	m_CanJump = false;
			// 	rb.ApplyForceAtCenter(new Vector2(0.0f, JumpForce), ForceMode2D.Force);
			// }

		}
		protected override void PhysicsUpdate() 
		{
			
		}

		protected override void OnCollisionBegin(Entity entity)
        {
            Log.Trace("collided");
            m_HasCollided = true;
        }

		protected override void OnCollisionEnd(Entity entity)
        {
            m_HasCollided = false;
        }
	}

}
