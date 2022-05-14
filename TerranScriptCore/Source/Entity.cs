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

		public Entity(byte[] uuidData)
		{
			id = new UUID(uuidData);
			transform = new Transform();
			transform.entity = this;
		}

		public static Entity FindWithName(string name)
		{
			byte[] entityID = Internal.Entity_FindEntityWithName_Internal(name);

			if (entityID != null)
				return new Entity(entityID);

			return null;
		}

		public static Entity FindWithID(UUID id)
		{
			byte[] entityID = Internal.Entity_FindEntityWithID_Internal(id);

			if (entityID != null)
				return new Entity(entityID);

			return null;
		}

		public static void Destroy(Entity entity) 
		{
			if (entity != null)
				Internal.Entity_DestroyEntity_Internal(entity.ID.Data);
		}

		public void AddComponent<T>() where T : Component
		{
			if (typeof(T) == typeof(Collider2D)) 
			{
				Log.Error("Can't add Collider2D to an entity because it is abstract!");
				return;
			}

			Internal.Entity_AddComponent_Internal(id.Data, typeof(T).FullName);
		} 

		public bool HasComponent<T>() where T : Component => Internal.Entity_HasComponent_Internal(id.Data, typeof(T).FullName);

		public void RemoveComponent<T>() where T : Component 
		{
			if(HasComponent<T>()) 
			{
				if (typeof(T) == typeof(Collider2D))
				{
					Log.Error("Can't remove Collider2D from an entity because it is abstract!");
					return;
				}

				Internal.Entity_RemoveComponent_Internal(id.Data, typeof(T).FullName);
			}
		} 

		public T GetComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
			{
				if (typeof(T).IsSubclassOf(typeof(Scriptable))) 
					return Internal.Entity_GetScriptableComponent_Internal(id.Data, typeof(T).FullName) as T;

				T component = new T();
				component.entity = this;
				return component;
			}

			return null;
		}		
	}
}
