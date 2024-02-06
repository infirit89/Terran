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
		
        protected override void Init()
		{
			//Console.WriteLine(this.Entity.ID);
			Console.WriteLine(string.Join(' ', TestArr));
			Array.Resize
		}

		protected override void Update(float deltaTime)
		{
		}
	}
}
