using System;
using System.Buffers;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terran;

namespace ScriptAssembly
{
	internal class Test : Scriptable
	{
		//public string[] Test1 = new string[] { "cum", "cum2", "cum3" };
		public int[] TestArr = new[] { 1, 2, 3, 4, 5 };
		public int[,] TestArr2 = new int[2, 2]
		{
			{ 1, 2 }, { 3, 4 }
		};
        public int[,,] TestArr3 = new int[2, 2, 2]
        {
            { {1, 2 }, {1, 2 }, }, { {3, 4 }, {3, 4 } }
        };


		public bool TestBool = false;
        public byte TestB = 10;
        public sbyte TestSB = -10;
		public ushort TestUS = 10;
        public short TestS = -10;
        public uint TestUI = 10;
        public int TestI = -10;
        public ulong TestUL = 10;
        public long TestL = -10;
        public float TestF = 10.10f;
        public double TestD = -100.0321d;
        public char TestC = 'c';
        public Vector2 TestVec2 = new Vector2(1.0f, 20.0f);
        public Vector3 TestVec3 = new Vector3(10.0312f, 123.0f, 2132f);
        public Color TestCol = new Color(1.0f, 0.5f, 0.0f, 1.0f);
        public string TestStr = "This is a test string! Will it show up?";

        protected override void Init()
		{
			//Console.WriteLine(this.Entity.ID);
			Console.WriteLine(TestArr.Length);
			Console.WriteLine(string.Join(' ', TestArr));
			Console.WriteLine(string.Join(' ', TestArr2));
            Console.WriteLine(string.Join(' ', TestArr3));

            Console.WriteLine(TestBool);
            Console.WriteLine(TestB);
            Console.WriteLine(TestSB);
            Console.WriteLine(TestUS);
            Console.WriteLine(TestS);
            Console.WriteLine(TestUI);
            Console.WriteLine(TestI);
            Console.WriteLine(TestUL);
            Console.WriteLine(TestL);
            Console.WriteLine(TestF);
            Console.WriteLine(TestD);
            Console.WriteLine(TestC);
            Console.WriteLine(TestVec2);
            Console.WriteLine(TestVec3);
            Console.WriteLine(TestCol);
            Console.WriteLine(TestStr);
        }

		protected override void Update(float deltaTime)
		{
		}
	}
}
