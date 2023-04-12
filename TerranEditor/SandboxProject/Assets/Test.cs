using System;
using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        public byte D = 1;
        public uint E = 1;
        public int A = 1;
		public string B = "Hello";
		public bool C = true;

        protected override void Init()
        {
            Log.Trace("Init");
        }

        protected override void Update(float deltaTime)
        {
            Log.Trace("Update");
        }
    }
}
