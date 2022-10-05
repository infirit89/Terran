namespace Terran
{
    public struct LayerMask
    {
        public ushort Value { get; set; }
        public string GetName() => Internal.LayerMask_GetName(Value);

        public static implicit operator ushort(LayerMask layerMask) => layerMask.Value;
    }
}
