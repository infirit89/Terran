using System;

namespace TerranScriptCore
{
	public class TestScript : Scriptable 
	{
		void Init() 
		{
			BoxCollider2D bc = entity.GetComponent<BoxCollider2D>();
			Log.Trace(bc.Offset);
			Log.Trace(bc.IsSensor);

			//CircleCollider2D cc = entity.GetComponent<CircleCollider2D>();
			//Log.Trace(cc.Offset);
			//Log.Trace(cc.IsSensor);
		}

		void Update() 
		{
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

		// Runs when the entity in the current scene, that has this script, is started
		public void Init() 
		{
			//RayCastHitInfo2D hitInfo = Physics2D.RayCast(entity.transform.Position, -Vector2.Up);
			rb = entity.GetComponent<Rigidbody2D>();
		}

		public void Update()
		{
			RayCastHitInfo2D hitInfo;
			float rayLength = (entity.transform.Scale.Y * 0.5f) + 0.1f;
			bool hasHit = Physics2D.RayCast(out hitInfo, entity.transform.Position, -Vector2.Up, rayLength);

			m_CanJump = hasHit;

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
				Log.Trace("jumped");
				rb.ApplyForceAtCenter(new Vector2(0.0f, JumpForce), ForceMode2D.Force);
			}

		}

		public void OnCollisionBegin(Entity entity) 
		{
			Log.Trace("collided");
			
		}
	}
}
