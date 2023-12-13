using Terran;

namespace Game
{
    class Test : Scriptable
    {
        protected override void Init() 
        {
            Log.Trace("Init");
        }

        protected override void Update(float deltaTime) 
        {
            Log.Trace("Update");
        }
    }
}