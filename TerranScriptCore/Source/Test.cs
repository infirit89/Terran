using System;
using System.Runtime.ConstrainedExecution;

namespace Terran
{
	public class TestScript : Scriptable
	{
		public bool BTest = false; 
		public char CTest = 'A';
		
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

		public Color ColorTest = Color.Cyan;
		
		protected override void Init() 
		{
			Log.Trace("bool: {0}", BTest);
			Log.Trace("char: {0}", CTest);
			
			Log.Trace("sbyte: {0}", I8Test);
			Log.Trace("short: {0}", I16Test);
			Log.Trace("int: {0}", I32Test);
			Log.Trace("long: {0}", I64Test);
			
			Log.Trace("byte: {0}", U8Test);
			Log.Trace("ushort: {0}", U16Test);
			Log.Trace("uint: {0}", U32Test);
			Log.Trace("ulong: {0}", U64Test);
			
			Log.Trace("float: {0}", FTest);
			Log.Trace("double: {0}", DTest);
			
			Log.Trace("String: {0}", StrTest);
			
			Log.Trace("Vector 2: {0}", Vec2Test);
			Log.Trace("Vector 3: {0}", Vec3Test);
			
			Log.Trace("Color: {0}", ColorTest);
		}

		protected override void Update() 
		{
			//entity.transform.Position += new Vector3(0.1f, 0.0f, 0.0f);
			Console.WriteLine("Test bruh bruh bruh");
			Log.Trace("Test");
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
			rb = entity.GetComponent<Rigidbody2D>();	
			Log.Trace(JumpForce);
		}

		protected override void Update()
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
