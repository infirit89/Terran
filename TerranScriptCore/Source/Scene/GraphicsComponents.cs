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
            get => Internal.CircleRenderer_GetColor(Entity.ID);
            set => Internal.CircleRenderer_SetColor(Entity.ID, value);
        }

        public float Thickness
        {
            get => Internal.CircleRenderer_GetThickness(Entity.ID);
            set => Internal.CircleRenderer_SetThickness(Entity.ID, value);
        }
    }

    public class Camera : Component
    {
        public bool IsPrimary
        {
            get => Internal.Camera_IsPrimary(Entity.ID);
            set => Internal.Camera_SetPrimary(Entity.ID, value);
        }

        public Color BackgroundColor
        {
            get => Internal.Camera_GetBackgroundColor(Entity.ID);
            set => Internal.Camera_SetBackgroundColor(Entity.ID, value);
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
