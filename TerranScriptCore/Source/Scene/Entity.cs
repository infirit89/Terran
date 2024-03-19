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

		public Entity[]? GetChildren()
		{
			//UUID[] childrenIDs = Internal.Entity_GetChildren(ID);

			//if (childrenIDs == null)
			//	return null;

			//Entity[] children = new Entity[childrenIDs.Length];

			//for (int i = 0; i < childrenIDs.Length; i++)
			//	children[i] = new Entity(childrenIDs[i]);

			//return children;
			return null;
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

				unsafe 
				{
					Internal.Entity_RemoveComponentICall(ID, typeof(T).GetHashCode());
				}
			}
		} 

		public T? GetComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
			{
				if (typeof(T).IsSubclassOf(typeof(Scriptable))) 
				{
					unsafe 
					{
						return Internal.Entity_GetScriptableComponentICall(ID) as T;
					}
				}

				T component = new T { Entity = new Entity(ID) };
				return component;
			}

			return null;
		}		
	}
}
