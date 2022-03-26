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
                Log.Trace(m_TestScriptable.Bruh2);
                Log.Trace(m_TestScriptable.TestStr);
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
        public int Test = 1;
        public bool Test1 = false;
        public string TestStr = "Hello";
        public bool Bruh2 = true;

        public Vector2 TestVec = new Vector2(1.0f, 0.7f);
        public Vector2 TestVec2 = new Vector2(1.0f, 0.7f);
        public Vector3 TestVec3 = new Vector3(-10.3f, -123.456f, 100.34f);
        public bool Bruh3 = true;

        // Runs when the entity in the current scene, that has this script, is started
        public void Init() 
        {
            // Prints statements, for test
            //Log.Warn("started");
            Log.Trace(TestVec);
            Log.Trace(TestVec3);
            Log.Trace(Test);

            if (entity.HasComponent<TestScriptable>())
                Log.Trace("yes");
             
            if (entity.HasComponent<TestScript>())
                Log.Trace("no");

            //entity.GetComponent<TestScriptable>();
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

        void BeginContact(Entity entity)
        {
            Log.Trace("test");
        }
    }
}
