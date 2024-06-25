using Terran;

namespace ScriptAssembly
{
    internal class SceneManagerTest : Scriptable
    {
        private string ScenePath = "Scenes/Scripting/physics";

        protected override void Init()
        {
            base.Init();
            SceneManager.LoadScene(ScenePath);
        }

        protected override void Update(float deltaTime)
        {
            base.Update(deltaTime);
        }
    }
}
