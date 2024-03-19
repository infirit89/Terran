using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terran;

namespace ScriptAssembly
{
	internal class Test2 : Scriptable
	{
		public int Test = 10;

        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            base.Update(deltaTime);
        }
    }
}
