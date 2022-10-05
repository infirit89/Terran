using System;
using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        private CapsuleCollider2D _CapsuleCollider;
        protected override void Init()
        {
            _CapsuleCollider = Entity.GetComponent<CapsuleCollider2D>();

            if (_CapsuleCollider != null)
                Log.Trace("cock");
        }

        protected override void Update()
        {
            Log.Trace(_CapsuleCollider.Offset);
            Log.Trace(_CapsuleCollider.Size);
            Log.Trace(_CapsuleCollider.IsSensor);

            if (Input.IsKeyPressed(KeyCode.I)) _CapsuleCollider.IsSensor = !_CapsuleCollider.IsSensor;

            if (Input.IsKeyPressed(KeyCode.W)) _CapsuleCollider.Offset += new Vector2(0.0f, 1.0f);
            else if (Input.IsKeyPressed(KeyCode.S)) _CapsuleCollider.Offset -= new Vector2(0.0f, 1.0f);

            if (Input.IsKeyPressed(KeyCode.D)) _CapsuleCollider.Offset += new Vector2(1.0f, 0.0f);
            else if (Input.IsKeyPressed(KeyCode.A)) _CapsuleCollider.Offset -= new Vector2(1.0f, 0.0f);

            if (Input.IsKeyPressed(KeyCode.Up)) _CapsuleCollider.Size += new Vector2(0.0f, 1.0f);
            else if (Input.IsKeyPressed(KeyCode.Down)) _CapsuleCollider.Size -= new Vector2(0.0f, 1.0f);

            if (Input.IsKeyPressed(KeyCode.Right)) _CapsuleCollider.Size += new Vector2(1.0f, 0.0f);
            else if (Input.IsKeyPressed(KeyCode.Left)) _CapsuleCollider.Size -= new Vector2(1.0f, 0.0f);
        }
    }
}