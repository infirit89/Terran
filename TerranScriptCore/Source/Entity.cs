using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Entity
    {
        public UUID ID
        {
            get => id;
        }

        public string Name
        {
            get => GetComponent<Tag>().Name;
            set => GetComponent<Tag>().Name = value;
        }

        private UUID id;
        public Transform transform;

        public Entity()
        {
            id = new UUID();
            transform = new Transform();
            transform.entity = this;
        }

        internal Entity(byte[] uuidData)
        {
            id = new UUID(uuidData);
            transform = new Transform();
            transform.entity = this;
        }

        public static Entity FindWithName(string name)
        {
            byte[] entityID = FindEntityWithName_Internal(name);

            if (entityID != null)
                return new Entity(entityID);

            return null;
        }

        public static Entity FindWithID(UUID id)
        {
            byte[] entityID = FindEntityWithID_Internal(id);

            if (entityID != null)
                return new Entity(entityID);

            return null;
        }

        public void AddComponent<T>() where T : Component => AddComponent_Internal(id.Data, typeof(T).FullName);

        public bool HasComponent<T>() where T : Component => HasComponent_Internal(id.Data, typeof(T).FullName);

        public void RemoveComponent<T>() where T : Component 
        {
            if(HasComponent<T>())
                RemoveComponent_Internal(id.Data, typeof(T).FullName);
        } 

        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                if (typeof(T).IsSubclassOf(typeof(Scriptable))) 
                    return GetScriptableComponent_Internal(id.Data, typeof(T).FullName) as T;

                T component = new T();
                component.entity = this;
                return component;
            }

            return null;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern bool HasComponent_Internal(byte[] runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void AddComponent_Internal(byte[] runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void RemoveComponent_Internal(byte[] runtimeID, string componentTypeStr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern object GetScriptableComponent_Internal(byte[] uuid, string moduleName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern byte[] FindEntityWithName_Internal(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern byte[] FindEntityWithID_Internal(UUID id);

        
    }
}
