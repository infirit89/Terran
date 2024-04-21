namespace Terran
{
    public struct LayerMask
    {
        public ushort Value { get; set; }
        public string Name 
        {
            get
            {
                unsafe
                {
                    return Internal.LayerMask_GetNameICall(Value)!;
                }
            }
        }

        public static implicit operator ushort(LayerMask layerMask) => layerMask.Value;
    }
}
