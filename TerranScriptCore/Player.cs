using System;

namespace TerranScriptCore
{
    class Player : Scriptable
    {
        void Init() 
        {

        }

        void Update() 
        {
            if (Input.KeyPressed(KeyCode.W)) 
            {
                entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
                Log.Trace("Moving up");
            }
            else if (Input.KeyPressed(KeyCode.S))
                entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);

            if (Input.KeyPressed(KeyCode.A))
                entity.transform.Position -= new Vector3(0.1f, 0.0f, 0.0f);
            else if (Input.KeyPressed(KeyCode.D))
                entity.transform.Position += new Vector3(0.1f, 0.0f, 0.0f);
        }
    }
}
