using Terran;

namespace ScriptAssembly
{
    internal class Test : Scriptable
    {
        public Entity Tomaaaaa;

        protected override void Init()
        {
            SpriteRenderer sp = Entity.GetComponent<SpriteRenderer>();
            Log.Trace(sp.Color);
        }

        protected override void Update(float deltaTime)
        {
            //if (Input.IsKeyDown(KeyCode.A))
            //    Log.Warn("A is down");

            //if (Input.IsKeyPressed(KeyCode.S))
            //    Log.Warn("S is pressed");

            //if (Input.IsKeyReleased(KeyCode.D))
            //    Log.Warn("D is pressed");

            //if (Input.IsMouseButtonDown(MouseButton.LeftButton))
            //    Log.Warn("Left button down");

            //if (Input.IsMouseButtonPressed(MouseButton.MiddleButton)) 
            //{
            //    Log.Warn("Middle button pressed");
            //    Log.Warn(Input.GetMousePosition());
            //}

            //if (Input.IsMouseButtonReleased(MouseButton.RightButton))
            //    Log.Warn("Right button released");

            //Log.Warn(Input.GetMousePosition());
            Log.Trace("chep");
        }
    }
}
