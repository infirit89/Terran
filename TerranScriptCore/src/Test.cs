using System;

namespace TerranScriptCore
{
    class Test
    {
        public static void Test1() 
        {
            Entity entity = Entity.FindWithName("Camera");

            if (entity != null)
                Console.WriteLine(entity.GetComponent<Transform>().Position);
            else
                Console.WriteLine("Couldn't find the entity");
        }
    }
}
