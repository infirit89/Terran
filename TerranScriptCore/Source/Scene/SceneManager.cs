using System;

namespace Terran
{
    public class SceneManager
    {
        public static void LoadScene(string scenePath)
        {
            unsafe
            {
                if (!Internal.SceneManager_LoadSceneICall(scenePath))
                    throw new ArgumentOutOfRangeException($"Couldn't load the scene: {scenePath}");
            }
        }
    }
}
