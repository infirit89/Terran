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
        public int test = 0;

        public void Init()
        {
            Log.Trace("started 2");
        }

        public void Update()
        {
            Log.Trace("updated");
            entity.GetComponent<Transform>().Position += new Vector3(0.1f, 0.0f, 0.0f);
        }
    }
}