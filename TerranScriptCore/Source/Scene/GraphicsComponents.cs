namespace Terran
{
    public class TextRenderer : Component
    {
        public Color Color
        {
            get => Internal.TextRenderer_GetColor(Entity.ID);
            set => Internal.TextRenderer_SetColor(Entity.ID, value);
        }

        public string Text
        {
            get => Internal.TextRenderer_GetText(Entity.ID);
            set => Internal.TextRenderer_SetText(Entity.ID, value);
        }
    }

    public class CircleRenderer : Component
    {
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
                    Internal.Camera_SetBackgroundColorICall(Entity.ID, value);
                }
            }
        }
    }

    public class SpriteRenderer : Component
    {
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
