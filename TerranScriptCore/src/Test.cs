using System;

namespace TerranScriptCore
{
    class Test
    {
        public static void Test1() 
        {
            Entity entity = new Entity(0);
            
            Console.WriteLine(entity.HasComponent<Transform>());

            entity.RemoveComponent<Transform>();
            Console.WriteLine(entity.HasComponent<Transform>());

            entity.AddComponent<Transform>();
            Console.WriteLine(entity.HasComponent<Transform>());
        }
    }
}
