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
		// Runs when the entity in the current scene, that has this script, is started
		public void Init() 
		{
			Log.Trace("test");
			Log.Error("error");
			Log.Warn("warn");
		}

		public void Update()
		{
			// Code to move the entity up and down
			/*if (Input.IsKeyPressed(KeyCode.W))
				// if w is pressed move the entity up one unit
				entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
			else if (Input.IsKeyPressed(KeyCode.S))
				// if s is pressed move the entity down one unit
				entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);*/

			float leftX = Input.GetControllerAxis(ControllerAxis.LeftX, 0);
			float leftY = Input.GetControllerAxis(ControllerAxis.LeftY, 0);


			leftX = (float)Math.Round(leftX, 1);
			leftY = (float)Math.Round(leftY, 1);

			Log.Trace(leftX);
			Log.Trace(leftY);

			if(leftX >= 0.5f || leftX <= -0.5f)
				entity.transform.Position += new Vector3((leftX * 0.1f), 0.0f, 0.0f);
			
			//if(leftY >= 0.5f || leftY <= -0.5f)
				//entity.transform.Position += new Vector3(0.0f, -(leftY * 0.1f), 0.0f);
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
