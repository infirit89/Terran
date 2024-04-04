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
                    return Internal.TextRenderer_GetColorICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.TextRenderer_SetColorICall(Entity.ID, in value);
                }
            }
        }

        public string Text
        {
            get
            {
                unsafe 
                {
                    return Internal.TextRenderer_GetTextICall(Entity.ID)!;
                }
            }
            set
            {
                unsafe 
                {
                    Internal.TextRenderer_SetTextICall(Entity.ID, value);
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
                    return Internal.CircleRenderer_GetColorICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.CircleRenderer_SetColorICall(Entity.ID, in value);
                }
            }
        }

        public float Thickness
        {
            get
            {
                unsafe
                {
                    return Internal.CircleRenderer_GetThicknessICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.CircleRenderer_SetThicknessICall(Entity.ID, value);
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
                    return Internal.Camera_IsPrimaryICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.Camera_SetPrimaryICall(Entity.ID, value);
                }
            }
        }

        public Color BackgroundColor
        {
            get
            {
                unsafe 
                {
                    return Internal.Camera_GetBackgroundColorICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.Camera_SetBackgroundColorICall(Entity.ID, in value);
                }
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
                    return Internal.SpriteRenderer_GetColorICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.SpriteRenderer_SetColorICall(Entity.ID, in value);
                }
            }
        }
    }
}
