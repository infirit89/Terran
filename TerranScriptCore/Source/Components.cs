﻿using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Component
    {
        public Entity entity;
    }

    public class Scriptable : Component
    {
        protected Scriptable() { }

        internal Scriptable(byte[] id) 
        {
            if (entity == null) 
                entity = new Entity(id);
        }
    }

    public class Tag : Component 
    {
        public string Name 
        {
            get
            {
                if (entity != null) 
                    return GetTagName_Internal(entity.ID.Data);

                // TODO: log that the entity is null
                return "";
            }

            set
            {
                if(entity != null)
                    SetTagName_Internal(entity.ID.Data, value);

                // TODO: log that the entity is null
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTagName_Internal(byte[] entityID, in string inName);
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern string GetTagName_Internal(byte[] entityID);
    }

    public class Transform : Component
    {
        public Vector3 Position
        {
            get
            {
                if (entity != null) 
                {
                    Vector3 outVec = GetTransformPosition_Internal(entity.ID.Data);
                    return outVec;
                }

                // TODO: log that the entity is null
                return new Vector3(0.0f, 0.0f, 0.0f);
            }

            set
            {
                if (entity != null) 
                {
                    SetTransformPosition_Internal(entity.ID.Data, value);
                }
                // TODO: log that the entity is null
            }
        }

        public Vector3 Rotation
        {
            get
            {
                if (entity != null) 
                    return GetTransformRotation_Internal(entity.ID.Data);

                // TODO: log that the entity is null
                return new Vector3(0.0f, 0.0f, 0.0f);
            }

            set
            {
                if (entity != null) 
                {
                    SetTransformRotation_Internal(entity.ID.Data, value);
                    return;
                }

                // TODO: log that the entity is null
            }
        }

        public Vector3 Scale
        {
            get
            {
                if (entity != null) 
                    return GetTransformScale_Internal(entity.ID.Data);

                // TODO: log that the entity is null
                return new Vector3(0.0f, 0.0f, 0.0f);
            }

            set
            {
                if (entity != null) 
                {
                    SetTransformScale_Internal(entity.ID.Data, value);
                    return;
                }

                // TODO: log that the entity is null
            }
        }

        // ---- position ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern Vector3 GetTransformPosition_Internal(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformPosition_Internal(byte[] entityID, Vector3 inPosition);
        // ------------------

        // ---- rotation ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern Vector3 GetTransformRotation_Internal(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformRotation_Internal(byte[] entityID, Vector3 inRotation);
        // ------------------

        // ---- scale ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern Vector3 GetTransformScale_Internal(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformScale_Internal(byte[] entityID, Vector3 inScale);
        // ---------------
    }
}
