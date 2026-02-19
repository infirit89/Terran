#include <LibCore/Event.h>
#include <LibCore/RefPtr.h>
#include <LibCore/UUID.h>

#include <LibScene/Components.h>
#include <LibScene/Entity.h>
#include <LibScene/Scene.h>

#include <LibScene/SceneEvent.h>
#include <gtest/gtest.h>

namespace Terran::World::Tests {

class SceneTest : public testing::Test {

protected:

    void on_scene_start(SceneStartSimulationEvent&) {
        m_is_scene_start_event_triggered = true;
    }

    void on_scene_stop(SceneStopSimulationEvent&) {
        m_is_scene_stop_event_triggered = true;
    }

    SceneTest()
        : m_scene(Core::RefPtr<Scene>::create(m_event_dispatcher))
    {
        m_event_dispatcher.handlers<SceneStartSimulationEvent>().connect<&SceneTest::on_scene_start>(this);
        m_event_dispatcher.handlers<SceneStopSimulationEvent>().connect<&SceneTest::on_scene_stop>(this);
    }
    virtual ~SceneTest() override = default;

    Core::EventDispatcher m_event_dispatcher;
    Core::RefPtr<Scene> m_scene;
    bool m_is_scene_start_event_triggered = false;
    bool m_is_scene_stop_event_triggered = false;
};

TEST_F(SceneTest, create_empty_entity_returns_valid_entity)
{
    Entity entity = m_scene->create_empty_entity();
    ASSERT_TRUE((bool)entity);
}

TEST_F(SceneTest, create_empty_entity_doesnt_add_tag_component)
{
    Entity entity = m_scene->create_empty_entity();
    ASSERT_FALSE(entity.has_component<TagComponent>());
}

TEST_F(SceneTest, create_empty_entity_doesnt_add_transform_component)
{
    Entity entity = m_scene->create_empty_entity();
    ASSERT_FALSE(entity.has_component<TransformComponent>());
}

TEST_F(SceneTest, create_empty_entity_doesnt_add_entity_to_scene_map)
{
    Entity entity = m_scene->create_empty_entity();
    ASSERT_TRUE(m_scene->entity_map().empty());
}

TEST_F(SceneTest, create_entity_without_name_returns_valid_entity)
{
    Entity entity = m_scene->create_entity();
    ASSERT_TRUE((bool)entity);
}

TEST_F(SceneTest, create_entity_with_empty_name_defaults_to_entity)
{
    Entity entity = m_scene->create_entity();
    ASSERT_EQ(entity.name(), "Entity");
}

TEST_F(SceneTest, create_entity_with_name_returns_valid_entity)
{
    Entity entity = m_scene->create_entity("TestEntity");
    ASSERT_TRUE((bool)entity);
}

TEST_F(SceneTest, create_entity_with_adds_tag_component)
{
    Entity entity = m_scene->create_entity("Tagged");
    ASSERT_TRUE(entity.has_component<TagComponent>());
}

TEST_F(SceneTest, create_entity_adds_transform_component)
{
    Entity entity = m_scene->create_entity("WithTransform");
    ASSERT_TRUE(entity.has_component<TransformComponent>());
}

TEST_F(SceneTest, create_entity_with_name_stores_correct_name)
{
    Entity entity = m_scene->create_entity("TestEntity");
    ASSERT_EQ(entity.name(), "TestEntity");
}

TEST_F(SceneTest, create_entity_with_explicit_uuid_sets_uuid)
{
    Core::UUID uuid;
    Entity entity = m_scene->create_entity("Identified", uuid);
    ASSERT_EQ(entity.id(), uuid);
}

TEST_F(SceneTest, create_entity_adds_entity_to_scene_map)
{
    Core::UUID uuid;
    m_scene->create_entity("Identified", uuid);
    Entity found = m_scene->find_entity(uuid);
    ASSERT_TRUE((bool)found);
}

TEST_F(SceneTest, destroy_entity_removes_it_from_entity_map)
{
    Entity entity = m_scene->create_entity("ToDestroy");
    Core::UUID uuid = entity.id();
    m_scene->destrory_entity(entity);
    Entity found = m_scene->find_entity(uuid);
    ASSERT_FALSE((bool)found);
}

TEST_F(SceneTest, destroy_entity_makes_it_invalid)
{
    Entity entity = m_scene->create_entity("ToDestroy");
    m_scene->destrory_entity(entity);
    ASSERT_FALSE(entity.valid());
}

TEST_F(SceneTest, destroy_entity_doesnt_erase_from_entity_map_if_entity_wasnt_registered_in_it)
{
    Entity entity = m_scene->create_entity("ToDestroy");
    Entity empty_entity = m_scene->create_empty_entity();
    ASSERT_FALSE(m_scene->entity_map().empty());
    m_scene->destrory_entity(empty_entity);
    ASSERT_FALSE(m_scene->entity_map().empty());
}

TEST_F(SceneTest, two_entities_have_different_uuids)
{
    Entity a = m_scene->create_entity("A");
    Entity b = m_scene->create_entity("B");
    ASSERT_NE(a.id(), b.id());
}

TEST_F(SceneTest, find_entity_by_uuid_returns_correct_entity)
{
    Entity entity = m_scene->create_entity("FindMe");
    Entity found = m_scene->find_entity(entity.id());
    ASSERT_EQ(entity, found);
}

TEST_F(SceneTest, find_entity_by_name_returns_correct_entity)
{
    Entity entity = m_scene->create_entity("FindByName");
    Entity found = m_scene->find_entity("FindByName");
    ASSERT_EQ(entity, found);
}

TEST_F(SceneTest, find_entity_by_invalid_uuid_returns_null_entity)
{
    Core::UUID nonexistent;
    Entity found = m_scene->find_entity(nonexistent);
    ASSERT_FALSE((bool)found);
}

TEST_F(SceneTest, find_entity_by_nonexistent_name_returns_null_entity)
{
    Entity found = m_scene->find_entity("DoesNotExist");
    ASSERT_FALSE((bool)found);
}

TEST_F(SceneTest, destroy_parent_also_destroys_children)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    Core::UUID child_uuid = child.id();
    m_scene->destrory_entity(parent);
    Entity found_child = m_scene->find_entity(child_uuid);
    ASSERT_FALSE((bool)found_child);
}

TEST_F(SceneTest, destroy_child_removes_itself_from_parent_children)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    m_scene->destrory_entity(child);
    ASSERT_TRUE(parent.children().empty());
}

TEST_F(SceneTest, duplicate_entity_creates_new_entity)
{
    Entity original = m_scene->create_entity("Original");
    Entity copy = m_scene->duplicate_entity(original);
    ASSERT_NE(original, copy);
}

TEST_F(SceneTest, duplicate_entity_copy_has_different_uuid)
{
    Entity original = m_scene->create_entity("Original");
    Entity copy = m_scene->duplicate_entity(original);
    ASSERT_NE(original.id(), copy.id());
}

TEST_F(SceneTest, duplicate_entity_copy_name_is_original_name_with_copy_suffix)
{
    Entity original = m_scene->create_entity("MyEntity");
    Entity copy = m_scene->duplicate_entity(original);
    ASSERT_EQ(copy.name(), "MyEntity Copy");
}

TEST_F(SceneTest, duplicate_entity_with_children_duplicates_children)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    Entity copy = m_scene->duplicate_entity(parent);
    ASSERT_EQ(copy.children().size(), 1u);
}

TEST_F(SceneTest, scene_is_not_playing_by_default)
{
    ASSERT_FALSE(m_scene->playing());
}

TEST_F(SceneTest, start_runtime_sets_playing_to_true)
{
    m_scene->start_runtime();
    ASSERT_TRUE(m_scene->playing());
}

TEST_F(SceneTest, start_runtime_triggers_scene_start_simulation_event)
{
    m_scene->start_runtime();
    ASSERT_TRUE(m_is_scene_start_event_triggered);
}

TEST_F(SceneTest, stop_runtime_sets_playing_to_false)
{
    m_scene->start_runtime();
    m_scene->stop_runtime();
    ASSERT_FALSE(m_scene->playing());
}

TEST_F(SceneTest, stop_runtime_triggers_scene_stop_simulation_event)
{
    m_scene->start_runtime();
    m_scene->stop_runtime();
    ASSERT_TRUE(m_is_scene_stop_event_triggered);
}

TEST_F(SceneTest, calling_start_runtime_twice_keeps_playing_true)
{
    m_scene->start_runtime();
    m_scene->start_runtime();
    ASSERT_TRUE(m_scene->playing());
}

TEST_F(SceneTest, calling_stop_runtime_without_starting_keeps_playing_false)
{
    m_scene->stop_runtime();
    ASSERT_FALSE(m_scene->playing());
}

}
