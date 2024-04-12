using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terran;

namespace TerranScriptCore.Source.Graphics
{
    public class Window
    {
        public static float Width 
        {
            get 
            {
                unsafe 
                {
                    return Internal.Window_GetWidthICall();
                }
            }
        }

        public static float Height 
        {
            get
            {
                unsafe 
                {
                    return Internal.Window_GetHeightICall();
                }
            }
        }

        public static bool VSync 
        {
            get
            {
                unsafe 
                {
                    return Internal.Window_IsVSyncICall();
                }
            }
        }

        public static Vector2 ContentScale 
        {
            get 
            {
                unsafe 
                {
                    return Internal.Window_GetContentScaleICall();
                }
            }
        }
    }
}
