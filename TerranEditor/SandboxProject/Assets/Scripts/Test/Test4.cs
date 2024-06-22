using Terran;

namespace ScriptAssembly
{
    internal class Test4 : Scriptable
    {
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.W))
            {
                Entity.GetComponent<TextRenderer>().Text = "That would be much appreciated";
            }
            if (Input.IsKeyPressed(KeyCode.A))
            {
                Entity.GetComponent<TextRenderer>().Text = "TOMAAAAAAAAAA";
            }

            if (Input.IsKeyDown(KeyCode.S))
            {
                Entity.GetComponent<TextRenderer>().Color = Color.Red;
            }
            if (Input.IsKeyReleased(KeyCode.S))
            {
                Entity.GetComponent<TextRenderer>().Color = Color.Blue;
            }

            base.Update(deltaTime);
        }
    }
}
