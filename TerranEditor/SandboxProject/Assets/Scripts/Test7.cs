using System;
using Terran;

namespace ScriptAssembly
{
    internal class Test7 : Scriptable
    {
        public float MouseSensitivity = 0.01f;
        private Camera Camera;
        protected override void Init()
        {
            Camera = Scene.MainCamera;
        }

        protected override void Update(float deltaTime)
        {
            Vector2 mousePosition = Input.GetMousePosition();

            var input = Camera.ScreenToWorldPoint(new Vector3(mousePosition.X, mousePosition.Y, 1.0f));

            Entity.Transform.Position = new Vector3(input.X, input.Y, 1);

            if (input.X % 2 == 0)
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Cyan;
            }
            else
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Blue;
            }

            if (input.Y % 2 == 0)
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Red;
            }
            else
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Magenta;
            }
        }
    }
}
