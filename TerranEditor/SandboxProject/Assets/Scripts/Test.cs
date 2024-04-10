using System;
using Terran;

namespace ScriptAssembly
{
#nullable enable
    internal class Test : Scriptable
    {
        //public string[] Test1 = new string[] { "cum", "cum2", "cum3" };
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

        public bool IsGrounded = false;

        protected override void Init()
<<<<<<< HEAD
		{
        }

		protected override void Update(float deltaTime)
		{
        }

        protected override void PhysicsUpdate()
        {
            Log.Trace("physics update");
        }

        protected override void OnCollisionBegin(Entity entity)
        {
            Log.Trace(entity.Name);
        }

        protected override void OnCollisionEnd(Entity entity)
        {
            Log.Trace(entity.Name);
        }
=======
        {
            RayCastHitInfo2D[] hitInfos = Physics2D.RayCastAll(Entity.Transform.Position, Vector2.Down, 30.0f);
            Log.Trace(hitInfos.Length);

            foreach (var hitInfo in hitInfos)
            {
                Log.Trace(hitInfo.Point);
                Log.Trace(hitInfo.Normal);

                if (hitInfo.Rigidbody != null)
                    Log.Trace(hitInfo.Rigidbody.Entity.Name);
            }
            Console.WriteLine("10");
            Log.Trace(1);
            Log.Warn(2);
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
            Log.Trace("chep");
        }
>>>>>>> 4ec76f3a0e9e85761747c042a8563ff94d1899d5
    }
#nullable disable
}
