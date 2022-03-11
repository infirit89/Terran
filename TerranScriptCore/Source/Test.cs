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

    public class TestScriptable 
    {
        public float Test = 1.0f;
        public int Test1 = 0;
        public int Test2 = 0;
        public int Test3 = 7;
        public int Test4 = 0;
        public int Test5 = 2;
        public int Test6 = 0;
        public int Test7 = 0;
        public int Test8 = 3;
        public int Test9 = 9;
        public int Test10 = 0;
        public int Test11 = 5;
        public int Test12 = 0;
        public int Test13 = 8;
        public int Test14 = 0;
        public int Test15 = 7;
        public int Test16 = 0;
        public int Test17 = 100;
        public int Test18 = 573;

        public float Bruh1 = 5.3f;

        public bool Bruh2 = true;

        public double Bruh3 = 6.7;

        public void Init() 
        {
            Log.Warn("started");
            Log.Trace(Test);
        }

        public void Update()
        {
            //if (Input.KeyPressed(KeyCode.W))
                //entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
            //else if (Input.KeyPressed(KeyCode.S))
                //entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
        }
    }
}
