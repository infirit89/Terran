﻿using System;

namespace TerranScriptCore
{
    class Test
    {
        public static void Test1()
        {
            Entity entity = Entity.FindWithName("Camera");
            Log.Trace(entity.GetComponent<Transform>().Position);
        }
    }

    public class TestScriptable : Scriptable 
    {
        public float Test = 1.0f;

        public void Init() 
        {
            Log.Warn("started");
            Log.Trace(Test);
        }

        public void Update()
        {
            if (Input.KeyPressed(KeyCode.W))
                entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
            else if (Input.KeyPressed(KeyCode.S))
                entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
        }
    }
}
