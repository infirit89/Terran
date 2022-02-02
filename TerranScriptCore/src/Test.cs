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
            Log.Trace(entity.HasComponent<Transform>());
        }

        public void Update()
        {
            entity.GetComponent<Transform>().Position += new Vector3(0.1f, 0.0f, 0.0f);


            Log.Trace("bruuh 2");
            Log.Trace("Update: {0}", entity.transform.Position);
        }
    }
}
