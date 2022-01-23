using System;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Entity
    {
        private uint m_RuntimeID;

        public Entity(uint id) 
        {
            m_RuntimeID = id;
        }

        public void AddComponent<T>() where T : Component
        {
            AddComponent_Internal(m_RuntimeID, typeof(T).FullName);
        }

        public bool HasComponent<T>() where T : Component
        {
            return HasComponent_Internal(m_RuntimeID, typeof(T).FullName);
        }

        public void RemoveComponent<T>() where T : Component 
        {
            RemoveComponent_Internal(m_RuntimeID, typeof(T).FullName);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern bool HasComponent_Internal(uint runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void AddComponent_Internal(uint runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void RemoveComponent_Internal(uint runtimeID, string componentTypeStr);
    }
}
