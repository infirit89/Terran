namespace Terran
{
    public class Scene
    {
        public static Camera MainCamera 
        {
            get 
            {
                unsafe 
                {
                    return new Camera(Internal.Scene_GetMainCameraICall());
                }
            }
        }
    }
}
