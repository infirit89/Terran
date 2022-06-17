using System.Runtime.CompilerServices;

namespace Terran
{
	public class Entity
	{
		public UUID ID
		{
			get => m_ID;
		}

		public string Name
		{
			get => GetComponent<Tag>().Name;
			set => GetComponent<Tag>().Name = value;
		}
		public Transform Transform => GetComponent<Transform>();

		private UUID m_ID;
		
		public Entity()
		{
			m_ID = new UUID();
		}

		public Entity(byte[] uuidData)
		{
			m_ID = new UUID(uuidData);
		}

		public static Entity FindWithName(string name)
		{
			byte[] entityID = Internal.Entity_FindEntityWithName(name);

			if (entityID != null)
				return new Entity(entityID);

			return null;
		}

		public static Entity FindWithID(UUID id)
		{
			byte[] entityID = Internal.Entity_FindEntityWithID(id);

			if (entityID != null)
				return new Entity(entityID);

			return null;
		}

		public static void Destroy(Entity entity) 
		{
			if (entity != null)
				Internal.Entity_DestroyEntity(entity.ID.Data);
		}

		public void AddComponent<T>() where T : Component
		{
			if (typeof(T) == typeof(Collider2D)) 
			{
				Log.Error("Can't add Collider2D to an entity because it is abstract!");
				return;
			}

			Internal.Entity_AddComponent(m_ID.Data, typeof(T).FullName);
		} 

		public bool HasComponent<T>() where T : Component => Internal.Entity_HasComponent(m_ID.Data, typeof(T).FullName);

		public void RemoveComponent<T>() where T : Component 
		{
			if(HasComponent<T>()) 
			{
				if (typeof(T) == typeof(Collider2D))
				{
					Log.Error("Can't remove Collider2D from an entity because it is abstract!");
					return;
				}

				Internal.Entity_RemoveComponent(m_ID.Data, typeof(T).FullName);
			}
		} 

		public T GetComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
			{
				if (typeof(T).IsSubclassOf(typeof(Scriptable))) 
					return Internal.Entity_GetScriptableComponent(m_ID.Data, typeof(T).FullName) as T;

				T component = new T();
				component.entity = this;
				return component;
			}

			return null;
		}		
	}
}
