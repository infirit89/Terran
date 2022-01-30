using System;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Entity
    {
        internal uint RuntimeID 
        {
            get 
            {
                return m_RuntimeID;
            }
        }

        private uint m_RuntimeID;

        public Entity(uint id) 
        {
            m_RuntimeID = id;
        }

        public static Entity FindWithName(string name) 
        {
            uint entityID = FindEntityWithName_Internal(name);

            if (entityID != 0xFFFFF)
                return new Entity(entityID);

            return null;
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

        public T GetComponent<T>() where T : Component, new() 
        {
            if (HasComponent<T>()) 
            {
                T component = new T();
                component.entity = this; 
                return component;
            }

            return null;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern bool HasComponent_Internal(uint runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void AddComponent_Internal(uint runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void RemoveComponent_Internal(uint runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern uint FindEntityWithName_Internal(string name);
    }
}
