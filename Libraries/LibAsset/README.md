# Asset System

## Requirements

- Generic
- Supports garbage collection
- Supports dependencies between assets

## Design

### System wide Definitions
1. `AssetId` = `Core::UUID`
2. `AssetTypeId` = `uint64_t`

### Asset Metadata

#### Properties
| Name | Type | Description |
|---|---|---|
| Id | `AssetId` | The storable asset identifier |
| Type | `AssetTypeId` | The asset type |
| Path | `filesystem::path` | The filesystem location of the asset |

### Asset (inherits Core::RefCounted)

#### Properties
| Name | Type | Description |
|---|---|---|
| Id | `AssetId` | The asset identifier |
| Type | `AssetTypeId` | The custom user defined type of the asset |

### Asset Error (interface)

#### Methods
| Name | Return Type | Description |
|---|---|---|
| message | `string_view` | The reason of the error |
| source | `string_view` | The source of the error |

### Asset Importer (interface)

#### Definitions
1. `AssetLoadResult` = `Core::Result<Core::RefPtr<Asset>, Core::Shared<AssetError>>`

#### Methods
| Name | Return Type | Parameters | Description |
|---|---|---|---|
| load | `AssetLoadResult` | `AssetMetadata` | Tries to load an asset, if successful the result will contain the loaded asset, if unsuccessful the result will contain an AssetError |
| save | `bool` | `AssetMetadata`, `Core::RefPtr<Asset>` | Tries to save an asset if unssucessful returns false |
| can_handle | `bool` | `filesystem::path` | Returns true if the importer can handle the specific file
| asset_type | `AssetTypeId` | None | Returns the asset type which the importer can handle |




## Notes

In the future the system should be able to:
1. Support async asset loading
2. Support preloading of assets
3. Support asset pinning

