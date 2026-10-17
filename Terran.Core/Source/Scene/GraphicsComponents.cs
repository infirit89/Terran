namespace Terran
{
    public class TextRenderer : Component
    {
        internal TextRenderer(UUID id)
            : base(id) { }

        public Color Color
        {
            get
            {
                unsafe 
                {
                    return Internal.TextRenderer_GetColorICall(Entity.Handle);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.TextRenderer_SetColorICall(Entity.Handle, in value);
                }
            }
        }

        public string Text
        {
            get
            {
                unsafe 
                {
                    return Internal.TextRenderer_GetTextICall(Entity.Handle)!;
                }
            }
            set
            {
                unsafe 
                {
                    Internal.TextRenderer_SetTextICall(Entity.Handle, value);
                }
            }
        }
    }

    public class CircleRenderer : Component
    {
        internal CircleRenderer(UUID id)
            : base(id) { }

        public Color Color
        {
            get
            {
                unsafe
                {
                    return Internal.CircleRenderer_GetColorICall(Entity.Handle);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.CircleRenderer_SetColorICall(Entity.Handle, in value);
                }
            }
        }

        public float Thickness
        {
            get
            {
                unsafe
                {
                    return Internal.CircleRenderer_GetThicknessICall(Entity.Handle);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.CircleRenderer_SetThicknessICall(Entity.Handle, value);
                }
            }
        }
    }

    public class Camera : Component
    {
        internal Camera(UUID id)
            : base(id) { }

        public bool IsPrimary
        {
            get
            {
                unsafe 
                {
                    return Internal.Camera_IsPrimaryICall(Entity.Handle);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.Camera_SetPrimaryICall(Entity.Handle, value);
                }
            }
        }

        public Color BackgroundColor
        {
            get
            {
                unsafe 
                {
                    return Internal.Camera_GetBackgroundColorICall(Entity.Handle);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.Camera_SetBackgroundColorICall(Entity.Handle, in value);
                }
            }
        }

        public Vector3 ScreenToWorldPoint(Vector3 screenPoint) 
        {
            unsafe 
            {
                return Internal.Camera_ScreenToWorldPointICall(Entity.Handle, screenPoint);
            }
        }
    }

    public class SpriteRenderer : Component
    {
        internal SpriteRenderer(UUID id) 
            : base(id) { }

        public Color Color
        {
            get
            {
                unsafe 
                {
                    return Internal.SpriteRenderer_GetColorICall(Entity.Handle);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.SpriteRenderer_SetColorICall(Entity.Handle, in value);
                }
            }
        }
    }
}
