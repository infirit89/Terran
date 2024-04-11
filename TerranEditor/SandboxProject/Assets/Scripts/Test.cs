using Terran;

namespace ScriptAssembly
{
    internal class Test : Scriptable
    {
        public Entity Tomaaaaa;

        protected override void Init()
<<<<<<< HEAD
		{
        }

		protected override void Update(float deltaTime)
		{
        }

        protected override void PhysicsUpdate()
=======
>>>>>>> 33ef74f (added mouse scene)
        {
            Test2 test2 = Tomaaaaa.GetComponent<Test2>();
            Log.Trace(test2.Test);
        }
<<<<<<< HEAD

        protected override void OnCollisionBegin(Entity entity)
        {
            Log.Trace(entity.Name);
        }

        protected override void OnCollisionEnd(Entity entity)
        {
            Log.Trace(entity.Name);
        }
=======
>>>>>>> 33ef74f (added mouse scene)
    }
}
