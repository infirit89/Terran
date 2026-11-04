using System;

namespace Terran
{
    public class SceneManager
    {
        /// <summary>
        /// Loads the scene by its path
        /// </summary>
        /// <param name="scenePath">The scene path relative to the project path</param>
        /// <exception cref="ArgumentOutOfRangeException">Gets thrown if no scene could be found or there was an error during the loading</exception>
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
