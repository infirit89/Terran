using System;
using System.Diagnostics;

namespace Terran
{
	public class TestScript : Scriptable 
	{
		void Init() 
		{
			/*BoxCollider2D bc = entity.GetComponent<BoxCollider2D>();
			Log.Trace(bc.Offset);
			Log.Trace(bc.IsSensor);*/

			Collider2D c = entity.GetComponent<Collider2D>();
			if(c != null)
				Log.Trace(c.Offset);

			//CircleCollider2D cc = entity.GetComponent<CircleCollider2D>();
			//Log.Trace(cc.Offset);
			//Log.Trace(cc.IsSensor
		}

		void Update() 
		{
			Log.Trace("Test");
		}

		void OnCollisionBegin(Entity entity) 
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
		public void Init() 
		{
			
			//RayCastHitInfo2D hitInfo = Physics2D.RayCast(entity.transform.Position, -Vector2.Up);
			rb = entity.GetComponent<Rigidbody2D>();
			
		}

		public void Update()
		{
			Stopwatch st = new Stopwatch();
			st.Start();
			RayCastHitInfo2D hitInfo;
			float rayLength = (entity.transform.Scale.Y * 0.5f) + 0.2f;
			bool hasHit = Physics2D.RayCast(out hitInfo, entity.transform.Position, -Vector2.Up, rayLength);

			m_CanJump = hasHit;

			if (hasHit && !m_HasCollided)
			{
				Log.Trace(hitInfo.Rigidbody.entity.Name);
				Log.Trace("ray hit");
			}

			// Code to move the entity up and down
			if (Input.IsKeyPressed(KeyCode.D))
				// if w is pressed move the entity up one unit
				entity.transform.Position += new Vector3(0.1f, 0.0f, 0.0f);
			else if (Input.IsKeyPressed(KeyCode.A))
				// if s is pressed move the entity down one unit
				entity.transform.Position -= new Vector3(0.1f, 0.0f, 0.0f);

			if (Input.IsKeyPressed(KeyCode.Space) && m_CanJump) 
			{
				m_CanJump = false;
				rb.ApplyForceAtCenter(new Vector2(0.0f, JumpForce), ForceMode2D.Force);
			}

			st.Stop();

			TimeSpan s = st.Elapsed;
			Log.Trace(s.TotalMilliseconds);

			Log.Trace(st.ElapsedMilliseconds);

		}
		public void PhysicsUpdate() 
		{
			
		}

		public void OnCollisionBegin(Entity entity) 
		{
			Log.Trace("collided");
			m_HasCollided = true;
		}

		public void OnCollisionEnd(Entity entity) 
		{
			m_HasCollided = false;
		}

	}
}
