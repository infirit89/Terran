using System;
using System.Runtime.CompilerServices;

namespace TestSandbox
{
    public class Class1
    {
        int test = 8;

        string test2 = "bruh";

        Class1() 
        {
            Console.WriteLine("bruh from Class 1");
            Console.WriteLine("In Class 1 test = " + test);
        }

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
            Console.WriteLine("Test 2 " + test);
        }

        public static void Test3(int a) 
        {
            Console.WriteLine("Test 3: " + a);
        }

        public void Test4() 
        {
            Console.WriteLine("Test 4: {0}", test2);
        }

        public static void Test5() 
        {
            Test5_Internal();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void Test5_Internal();
    }
}
