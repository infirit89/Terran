using System;
using System.Buffers;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using Terran;

namespace ScriptAssembly
{
#nullable enable
    internal class Test : Scriptable
	{
        //public string[] Test1 = new string[] { "cum", "cum2", "cum3" };
        public int[] TestArr = new[] { 1, 2, 3, 4, 5 };
        public Entity[] TestEntityArr = new Entity[6];
        public int[,] TestArr2 = new int[2, 2]
        {
            { 1, 2 }, { 3, 4 }
        };
        public int[,,] TestArr3 = new int[2, 2, 2]
        {
            { { 1, 2 }, { 1, 2 }, }, { { 3, 4 }, { 3, 4 } }
        };


        //public bool TestBool = false;
        //      public byte TestB = 10;
        //      public sbyte TestSB = -10;
        //public ushort TestUS = 10;
        //      public short TestS = -10;
        //      public uint TestUI = 10;
        //      public int TestI = -10;
        //      public ulong TestUL = 10;
        //      public long TestL = -10;
        //      public float TestF = 10.10f;
        //      public double TestD = -100.0321d;
        //      public char TestC = 'c';
        //      public Vector2 TestVec2 = new Vector2(1.0f, 20.0f);
        //      public Vector3 TestVec3 = new Vector3(10.0312f, 123.0f, 2132f);
        //      public Color TestCol = new Color(1.0f, 0.5f, 0.0f, 1.0f);
        //      public string TestStr = "This is a test string! Will it show up?";
        //      public Entity TestEntity;

        protected override void Init()
		{
            CircleCollider2D? collider = Entity.GetComponent<CircleCollider2D>();
            if (collider == null) 
            {
                Log.Error("collider is null");
                return;
            }

            Log.Trace(collider.Radius);
            collider.Radius *= 2.0f;
            
            //Log.Trace(rb.SleepState);
            //rb.SleepState = RigidbodySleepState.NeverSleep;

            //Log.Trace(rb.GravityScale);
            //rb.GravityScale += 10.0f;

            //TextRenderer? cr = Entity.GetComponent<TextRenderer>();
            //Log.Trace(cr.Color);
            //cr.Color = Color.Cyan;

            //Log.Trace(cr.Text);
            //cr.Text = "Test test test this is a test";
        }

		protected override void Update(float deltaTime)
		{
            //if (Input.IsKeyDown(KeyCode.A))
            //    Log.Warn("A is down");

            //if (Input.IsKeyPressed(KeyCode.S))
            //    Log.Warn("S is pressed");

            //if (Input.IsKeyReleased(KeyCode.D))
            //    Log.Warn("D is pressed");

            //if (Input.IsMouseButtonDown(MouseButton.LeftButton))
            //    Log.Warn("Left button down");

            //if (Input.IsMouseButtonPressed(MouseButton.MiddleButton)) 
            //{
            //    Log.Warn("Middle button pressed");
            //    Log.Warn(Input.GetMousePosition());
            //}

            //if (Input.IsMouseButtonReleased(MouseButton.RightButton))
            //    Log.Warn("Right button released");

            //Log.Warn(Input.GetMousePosition());
        }
	}
#nullable disable
}
