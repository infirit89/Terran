﻿using System;
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
		public Vector2[] Test3;
		public string Test2 = "cum";

		protected override void Init()
		{
			Log.Warn("initialize");
		}

		protected override void Update(float deltaTime)
		{
			Log.Trace("update");

			Log.Trace(Entity.Transform is null);
			Log.Trace(string.Join(", ", Test3));
		}
	}
}
