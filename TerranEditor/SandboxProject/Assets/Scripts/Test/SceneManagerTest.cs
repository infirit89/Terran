using Terran;

namespace ScriptAssembly
{
    [SourceInfo]
    internal class SceneManagerTest : Scriptable
    {
        private string ScenePath = "Scenes/Physics/RayCastTest";

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
