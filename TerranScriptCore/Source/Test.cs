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
        public int iTest = 100;
        public bool bTest = true;

        public void Update()
        {
            entity.GetComponent<Transform>().Position += new Vector3(0.1f, 0.0f, 0.0f);
        }
    }
}
