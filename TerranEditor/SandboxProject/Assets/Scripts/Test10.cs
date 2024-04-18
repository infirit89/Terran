using System;
using Terran;

namespace ScriptAssembly
{
    internal class Test10 : Scriptable
    {
        private Random _Random = new Random();
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            var item = Physics2D.RayCastAll(new Vector2(0, 0), new Vector2(10, 0));

            foreach (var thing in item)
            {
                if (thing)
                {
                    Log.Warn("Hit entity {0}", thing.Rigidbody.Entity.Name);

                    float r = (float)_Random.Next(0, 255) / 255;
                    float g = (float)_Random.Next(0, 255) / 255;
                    float b = (float)_Random.Next(0, 255) / 255;

                    thing.Rigidbody.Entity.GetComponent<SpriteRenderer>().Color = new Color(r, g, b);
                }
            }
        }
    }
}
