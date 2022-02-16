using System;

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
        public void Init() 
        {
            Log.Warn("start");
        }

        public void Update()
        {
            if (Input.KeyPressed(KeyCode.A))
                Log.Trace("test");
        }
    }
}
