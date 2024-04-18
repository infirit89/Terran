using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Terran
{
    public class Entity
    {
        public UUID ID
        {
            get => m_ID;
        }

        private UUID m_ID;

        public string Name
        {
            get => GetComponent<Tag>()!.Name;
            set => GetComponent<Tag>()!.Name = value;
        }
        public Transform Transform => GetComponent<Transform>()!;

        public Entity()
        {
            m_ID = new UUID();
        }

        internal Entity(UUID id)
        {
            m_ID = id;
            //m_ID = new UUID(uuidData);
        }

        public static Entity? FindWithName(string name)
        {
            unsafe
            {
                if (!Internal.Entity_FindEntityWithNameICall(name, out var id))
                    return null;

                Log.Trace(id);

                return new Entity(id);
            }
        }

        public static void Destroy(Entity entity)
        {
            unsafe
            {
                if (entity != null)
                    Internal.Entity_DestroyEntityICall(entity.ID);
            }
        }

        public void AddComponent<T>() where T : Component
        {
            if (typeof(T) == typeof(Collider2D))
            {
                Log.Error("Can't add Collider2D to an entity because it is abstract!");
                return;
            }

            unsafe
            {
                Internal.Entity_AddComponentICall(ID, typeof(T).GetHashCode());
            }
        }

        //public Entity[]? GetChildren()
        //{
        //    unsafe
        //    {
        //        IntPtr childrenIdsHandle = Internal.Entity_GetChildrenICall(in m_ID);
        //        if (childrenIdsHandle == IntPtr.Zero)
        //            return null;

        //        UUID[]? childrenIds = GCHandle.FromIntPtr(childrenIdsHandle).Target as UUID[];
        //        if (childrenIds == null)
        //            return null;

        //        Entity[] children = new Entity[childrenIds!.Length];

        //        for (int i = 0; i < childrenIds.Length; i++)
        //            children[i] = new Entity(childrenIds[i]);
        //        return children;
        //    }
        //}

        public bool HasComponent<T>() where T : Component
        {
            unsafe
            {
                return Internal.Entity_HasComponentICall(in m_ID, typeof(T).GetHashCode());
            }
        }

        public void RemoveComponent<T>() where T : Component
        {
            if (HasComponent<T>())
            {
                if (typeof(T) == typeof(Collider2D))
                {
                    Log.Error("Can't remove Collider2D from an entity because it is abstract!");
                    return;
                }

                unsafe
                {
                    Internal.Entity_RemoveComponentICall(ID, typeof(T).GetHashCode());
                }
            }
        }

        public T GetComponent<T>() where T : Component
        {
            if (HasComponent<T>())
            {
                if (typeof(T).IsSubclassOf(typeof(Scriptable)))
                {
                    unsafe
                    {
                        IntPtr handle = Internal.Entity_GetScriptableComponentICall(ID);
                        if (handle == IntPtr.Zero)
                            throw new NullReferenceException($"The entity doesn't have {typeof(T).FullName}");

                        GCHandle gcHandle = GCHandle.FromIntPtr(handle);

                        if (!(gcHandle.Target is T))
                            throw new NullReferenceException($"The entity doesn't have {typeof(T).FullName}");

                        return (T)gcHandle.Target;
                    }
                }

                T? component = (T?)Activator.CreateInstance(typeof(T), BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance, null, new object[] { ID }, null);

                if (component == null)
                    throw new NullReferenceException($"The entity doesn't have {typeof(T).FullName}");

                return component;
            }

            throw new NullReferenceException($"The entity doesn't have {typeof(T).FullName}");
        }

        public int ChildrenCount
        {
            get
            {
                unsafe
                {
                    return Internal.Entity_GetChildrenCountICall(ID);
                }
            }
        }

        public Entity GetChild(int index)
        {
            unsafe
            {
                if (index < 0 || index >= ChildrenCount)
                    throw new ArgumentOutOfRangeException("Index is out of range");

                return new Entity(Internal.Entity_GetChildICall(ID, index));
            }
        }

    }
}
