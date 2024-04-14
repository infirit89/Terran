using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Terran
{
    [AttributeUsage(AttributeTargets.Field)]
    public class SerializeFieldAttribute : Attribute 
    {
    }
}
