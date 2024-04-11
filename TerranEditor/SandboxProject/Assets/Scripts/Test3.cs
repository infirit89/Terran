using System;
using Terran;

namespace ScriptAssembly
{
    internal class Test3 : Scriptable
    {
        private static int _MaxCount = 1024;
        private int _Index = 0;
        private Vector3[] _Vectors = new Vector3[_MaxCount];
        private Random _Random;
        protected override void Init()
        {
            _Random = new Random();
            for (int i = 0; i < _Vectors.Length; i++)
            {
                _Vectors[i] = new Vector3(_Random.Next(1, 10), _Random.Next(1, 10), _Random.Next(1, 10));
            }
        }

        protected override void Update(float deltaTime)
        {
            if (_Index == _MaxCount - 1)
            {
                _Index = 0;
            }

            Log.Trace(_Index);
            //Log.Trace(_Vectors[_Index]);

            Entity.GetComponent<SpriteRenderer>().Color = _Index % 2 == 0 ? Color.Green : Color.Blue;
            Entity.Transform.Position = _Vectors[_Index];
            Log.Trace(Entity.Transform.IsDirty);
            Log.Trace(Entity.Transform.IsDirty);

            _Index++;
        }
    }
}
