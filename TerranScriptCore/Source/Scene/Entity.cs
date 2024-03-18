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
			//byte[] entityID = Internal.Entity_FindEntityWithName(name);

			//if (entityID != null)
			//	return new Entity(entityID);

			return null;
		}

		internal static Entity? FindWithID(UUID id)
		{
			return Internal.Entity_FindEntityWithID(id) ? new Entity(id) : null;
		}

		public static void Destroy(Entity entity) 
		{
			//if (entity != null)
			//	Internal.Entity_DestroyEntity(entity.ID.Data);
		}

		public void AddComponent<T>() where T : Component
		{
			if (typeof(T) == typeof(Collider2D)) 
			{
				Log.Error("Can't add Collider2D to an entity because it is abstract!");
				return;
			}

			//Internal.Entity_AddComponent(ID.Data, typeof(T));
		}

		public Entity[]? GetChildren()
		{
			UUID[] childrenIDs = Internal.Entity_GetChildren(ID);

			if (childrenIDs == null)
				return null;
			
			Entity[] children = new Entity[childrenIDs.Length];

			for (int i = 0; i < childrenIDs.Length; i++)
				children[i] = new Entity(childrenIDs[i]);

			return children;
		}

		public bool HasComponent<T>() where T : Component 
		{
			unsafe 
			{
				return Internal.Entity_HasComponentICall(in m_ID, typeof(T).GetHashCode());			
			}
		}

		public void RemoveComponent<T>() where T : Component 
		{
			if(HasComponent<T>()) 
			{
				if (typeof(T) == typeof(Collider2D))
				{
					Log.Error("Can't remove Collider2D from an entity because it is abstract!");
					return;
				}

				//Internal.Entity_RemoveComponent(ID.Data, typeof(T));
			}
		} 

		public T? GetComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
			{
				//if (typeof(T).IsSubclassOf(typeof(Scriptable))) 
				//	return Internal.Entity_GetScriptableComponent(ID.Data) as T;

				T component = new T { Entity = new Entity(ID) };
				return component;
			}

			return null;
		}		
	}
}
