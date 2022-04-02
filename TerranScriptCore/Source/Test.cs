using System;

namespace TerranScriptCore
{
	public class TestScript : Scriptable 
	{
		private TestScriptable m_TestScriptable;
		private TestScript m_TestScript;

		void Init() 
		{
			Entity entity = Entity.FindWithName("Test Entity");

			m_TestScriptable = entity.GetComponent<TestScriptable>();
			m_TestScript = entity.GetComponent<TestScript>();

			if (m_TestScript == null)
				Log.Trace("test script is fucking null bastard");

			if (m_TestScriptable == null)
				Log.Trace("null bitch");
			else 
			{
				//Log.Trace(m_TestScriptable.Bruh2);
				//Log.Trace(m_TestScriptable.TestStr);
			}

			Entity entity2 = Entity.FindWithName("Test 2");
			entity2.AddComponent<TestScriptable>();
		}

		void Update() 
		{
		}
	}

	// Test script
	public class TestScriptable : Scriptable
	{
		private Rigidbody2D rb;
		
		// Runs when the entity in the current scene, that has this script, is started
		public void Init() 
		{
			rb = entity.GetComponent<Rigidbody2D>();
			
			if(rb != null)
				rb.ApplyForceAtCenter(new Vector2(10.0f, 100.0f));

			BoxCollider2D bc = entity.GetComponent<BoxCollider2D>();

			if (bc != null)
				Log.Trace("Offset: {0}", bc.Offset);

			CircleCollider2D cc = entity.GetComponent<CircleCollider2D>();

			if (cc != null)
				Log.Trace("Offset: {0}", cc.Offset);
		}

		public void Update()
		{
			// Code to move the entity up and down
			if (Input.KeyPressed(KeyCode.W))
				// if w is pressed move the entity up one unit
				entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
			else if (Input.KeyPressed(KeyCode.S))
				// if s is pressed move the entity down one unit
				entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
		}

		void OnCollisionBegin(Entity entity)
		{
			Log.Trace("Begin contact");
		}

		void OnCollisionEnd(Entity entity) 
		{
			Log.Trace("End contact");
		}
	}
}
