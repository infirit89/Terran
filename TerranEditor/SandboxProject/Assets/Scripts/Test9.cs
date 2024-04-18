using System;
using Terran;

namespace ScriptAssembly
{
    internal class Test9 : Scriptable
    {
        private Random _Random = new Random();
        protected override void Init()
        {
        }

        protected override void Update(float deltaTime)
        {
            var item = Physics2D.RayCast(new Vector2(5, 0), new Vector2(5, 20));

            if (item)
            {
                Log.Warn("Hit entity {0}", item.Rigidbody.Entity.Name);

                float r = (float)_Random.Next(0, 255) / 255;
                float g = (float)_Random.Next(0, 255) / 255;
                float b = (float)_Random.Next(0, 255) / 255;

                item.Rigidbody.Entity.GetComponent<SpriteRenderer>().Color = new Color(r, g, b);

            }
        }
    }
}
