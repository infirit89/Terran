#include <LibAsset/Asset.h>
#include <LibAsset/AssetImporter.h>
#include <LibAsset/AssetImporterRegistry.h>
#include <LibAsset/AssetMetadata.h>
#include <LibAsset/AssetTypes.h>
#include <LibAsset/AssetImporterError.h>

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/RefPtr.h>
#include <LibCore/UUID.h>

#include <gtest/gtest.h>

#include <filesystem>

namespace Terran::Asset::Tests {

class MockAsset final : public Asset {
public:
    MockAsset(AssetId const& handle)
        : Asset(handle)
    {
    }
    TR_DECLARE_ASSET_TYPE(MockAsset);
};

class MockAsset2 final : public Asset {
public:
    MockAsset2(AssetId const& handle)
        : Asset(handle)
    {
    }
    TR_DECLARE_ASSET_TYPE(MockAsset2);
};

class MockAssetImporter final : public AssetImporter {
public:
    virtual ~MockAssetImporter() override = default;
    [[nodiscard]] virtual AssetLoadResult load(AssetMetadata const& assetMetadata) override
    {
        Core::RefPtr<MockAsset> asset = Core::RefPtr<MockAsset>::create(assetMetadata.AssetId);
        return { asset };
    }
    virtual bool save(AssetMetadata const&, Core::RefPtr<Asset> const&) override
    {
        return true;
    }
    [[nodiscard]] virtual bool can_handle(std::filesystem::path const&) override
    {
        return true;
    }
    [[nodiscard]] virtual AssetTypeId asset_type() override
    {
        return MockAsset::static_type();
    }
};
class AssetImporterRegistryTest : public testing::Test {
protected:
    AssetImporterRegistryTest()
    {
        m_asset_metadata.AssetId = Core::UUID();
        m_asset_metadata.Path = "";
        m_asset_metadata.Type = MockAsset::static_type();

        m_asset_importer = Core::CreateShared<MockAssetImporter>();
        AssetImporterRegistry::register_asset<MockAsset>(m_asset_importer);
    }
    virtual ~AssetImporterRegistryTest() override
    {
        AssetImporterRegistry::clear();
    }

    AssetMetadata m_asset_metadata;
    Core::Shared<MockAssetImporter> m_asset_importer;
};

TEST_F(AssetImporterRegistryTest, load_calls_load_for_a_given_asset_importer_when_it_is_registered)
{
    auto assetRes = AssetImporterRegistry::load(m_asset_metadata);
    ASSERT_TRUE(assetRes.is_ok());
    ASSERT_TRUE(assetRes.value()->is_valid());
    ASSERT_EQ(assetRes.value()->id(), m_asset_metadata.AssetId);
}

TEST_F(AssetImporterRegistryTest, load_returns_error_when_asset_importer_of_a_given_type_is_not_registered)
{
    AssetMetadata metadata;
    metadata.Type = MockAsset2::static_type();
    auto assetRes = AssetImporterRegistry::load(metadata);
    ASSERT_FALSE(assetRes.is_ok());
    Core::Shared<AssetImporterError> importerError = Core::CreateShared<AssetImporterError>(AssetImporterError::Code::ImporterNotFound);
    ASSERT_EQ(assetRes.error()->message(), importerError->message());
}

TEST_F(AssetImporterRegistryTest, save_calls_save_for_a_given_asset_importer_when_it_is_registered)
{
    Core::RefPtr<MockAsset> mockAsset = Core::RefPtr<MockAsset>::create(m_asset_metadata.AssetId);
    bool result = AssetImporterRegistry::save(m_asset_metadata, mockAsset);
    ASSERT_TRUE(result);
}


TEST_F(AssetImporterRegistryTest, save_returns_false_when_asset_importer_of_a_given_type_is_not_registered)
{
    Core::RefPtr<MockAsset2> mockAsset = Core::RefPtr<MockAsset2>::create(Core::UUID());
    AssetMetadata metadata;
    metadata.Type = mockAsset->type();
    bool result = AssetImporterRegistry::save(metadata, mockAsset);
    ASSERT_FALSE(result);
}

TEST_F(AssetImporterRegistryTest, save_returns_false_when_assetmetadata_type_and_asset_type_dont_match)
{
    Core::RefPtr<MockAsset2> mockAsset = Core::RefPtr<MockAsset2>::create(Core::UUID());
    AssetMetadata metadata;
    metadata.Type = MockAsset::static_type();
    bool result = AssetImporterRegistry::save(metadata, mockAsset);
    ASSERT_FALSE(result);
}

}
