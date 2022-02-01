using System;
using System.Runtime.CompilerServices;

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

    class TestScriptable : Scriptable 
    {
        public override void Init()
        {
            Log.Trace("bryh");
            Log.Trace(entity.HasComponent<Transform>());
        }

        public override void Update()
        {
            Log.Trace("Updated");
        }
    }
}
