﻿using System;
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
            Log.Trace(this.Entity.ID);
            Log.Trace(string.Join(' ', TestArr));
            Log.Trace(string.Join(' ', TestArr2));
            Log.Trace(string.Join(' ', TestArr3));

            Log.Trace(TestBool);
            Log.Trace(TestB);
            Log.Trace(TestSB);
            Log.Trace(TestUS);
            Log.Trace(TestS);
            Log.Trace(TestUI);
            Log.Trace(TestI);
            Log.Trace(TestUL);
            Log.Trace(TestL);
            Log.Trace(TestF);
            Log.Trace(TestD);
            Log.Trace(TestC);
            Log.Trace(TestVec2);
            Log.Trace(TestVec3);
            Log.Trace(TestCol);
            Log.Trace(TestStr);
        }

		protected override void Update(float deltaTime)
		{
		}
	}
}