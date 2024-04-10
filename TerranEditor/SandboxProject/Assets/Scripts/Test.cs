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
    }
#nullable disable
}
