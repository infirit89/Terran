using System;

namespace TestSandbox
{
    public class Class1
    {
        public static void Test() 
        {
            // divide by zero exception:
            int a = 10;
            a -= 10;
            int b = 10 / a;
            Console.WriteLine(b);
        }

        public void Test2() 
        {
            Console.WriteLine("Test 2");
        }

        public static void Test3(int a) 
        {
            Console.WriteLine("Test 3: " + a);
        }
    }
}
