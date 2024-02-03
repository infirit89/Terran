using System;
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
		public byte TestB;
        public ushort TestUS;
        public uint TestUI;
        public ulong TestUL;

        public sbyte TestSB;
        public short TestS;
        public int TestI;
        public long TestL;

        public float TestF;
        public double TestD;

        public bool TestBool;
        public char TestC;

        public Vector2 TestV2;
        public Vector3 TestV3;
        public Color TestCol;
        public Entity TestE;

        protected override void Init()
		{
			Console.WriteLine(this.Entity.ID);
		}

		protected override void Update(float deltaTime)
		{
		}
	}
}
