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
		protected override void Init()
		{
			Log.Warn("initialize");
		}

		protected override void Update(float deltaTime)
		{
			Log.Trace("update");

			Log.Trace(Entity.Transform is null);
		}
	}
}
