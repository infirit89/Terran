using Terran;

namespace ScriptAssembly
{
    internal class Test6 : Scriptable
    {
        protected override void Init()
        {

        }

        protected override void Update(float deltaTime)
        {
            Vector2 position = Input.GetMousePosition();

            Log.Trace(position);

            if (position.X % 2 == 0)
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Red;
            }
            else
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Green;
            }

            if (Input.IsMouseButtonDown(MouseButton.LeftButton))
            {
                Entity.Transform.Position += new Vector3(-10, 0, 0) * deltaTime;
            }
            else if (Input.IsMouseButtonDown(MouseButton.RightButton))
            {
                Entity.Transform.Position += new Vector3(10, 0, 0) * deltaTime;
            }

            if (Input.IsMouseButtonPressed(MouseButton.MiddleButton))
            {
                Entity.Transform.Position += new Vector3(0, 10, 0) * deltaTime;
            }
            else if (Input.IsMouseButtonReleased(MouseButton.MiddleButton))
            {
                Entity.Transform.Position += new Vector3(0, -10, 0) * deltaTime;
            }

        }
    }
}
