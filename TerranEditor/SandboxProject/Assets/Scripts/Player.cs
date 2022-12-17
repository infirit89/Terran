using Terran;

namespace ScriptAssembly
{
    internal class Player : Scriptable
    {
        public float Speed = 1.0f;
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.W))
                Entity.Transform.Position += new Vector3(0.0f, Speed, 0.0f);
            else if(Input.IsKeyPressed(KeyCode.S))
                Entity.Transform.Position -= new Vector3(0.0f, Speed, 0.0f);
        }
    }
}
