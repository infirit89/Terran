using System;

namespace TerranScriptCore
{
    // Test script
    public class TestScriptable : Scriptable
    {
        public int Test = 1;
        public bool Test1 = false;
        public string TestStr = "Hello";
        public bool Bruh2 = true;

        public Vector2 TestVec = new Vector2(1.0f, 0.7f);

        // Runs when the entity in the current scene, that has this script is started
        public void Init() 
        {
            // Prints statements, for test
            //Log.Warn("started");
            Log.Trace(TestVec);
            Log.Trace(Test);
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
    }
}
