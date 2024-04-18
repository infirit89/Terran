using Terran;

namespace ScriptAssembly
{
    internal class Test8 : Scriptable
    {
        private Entity Entity;
        protected override void Init()
        {
            Entity = Entity.FindWithName("Tomaaa");

            if (!Entity.HasComponent<TextRenderer>())
            {
                Entity.AddComponent<TextRenderer>();
            }

            Entity.GetComponent<TextRenderer>().Text = "Test";
            Entity.GetComponent<TextRenderer>().Color = Color.Blue;
            Entity.Name = "Test";
        }

        protected override void Update(float deltaTime)
        {
            var children = Entity.GetChildren();
            if (children == null)
                Log.Trace("maikati e momje denislaveeeeeeeeeeeeeeeee");

            if (Input.IsMouseButtonDown(MouseButton.LeftButton))
            {
                Entity.Destroy(children[1]);
            }
            if (Input.IsMouseButtonDown(MouseButton.RightButton))
            {
                Entity.Destroy(children[0]);
            }
        }
    }
}
