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

            for (int i = 0; i < Entity.ChildrenCount; i++)
                Log.Trace(Entity.GetChild(i).Name);
        }

        protected override void Update(float deltaTime)
        {
            //if (Input.IsMouseButtonDown(MouseButton.LeftButton))
            //{
            //    Entity.Destroy(Entity.GetChild(1));
            //}
            //if (Input.IsMouseButtonDown(MouseButton.RightButton))
            //{
            //    Entity.Destroy(Entity.GetChild(0));
            //}
        }
    }
}
