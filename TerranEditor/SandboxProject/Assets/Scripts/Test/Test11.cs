using ScriptAssembly.Assets.Scripts.Test;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Terran;

namespace ScriptAssembly
{
    internal class Test11 : Scriptable
    {
        private string ScenePath2 = "Scenes/TestScene16";

        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.W))
            {
                StaticCounter.Counter++;
                Log.Trace(StaticCounter.Counter);
                SceneManager.LoadScene(ScenePath2);
            }
        }
    }
}
