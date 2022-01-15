using System;

namespace TestSandbox
{
    public class Class1
    {
        int test = 8;

        int test2 
        {
            get; set;
        }

        Class1() 
        {
            test2 = 10;
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
    }
}
