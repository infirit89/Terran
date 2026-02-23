#include <LibCore/Event.h>
#include <LibCore/RefPtr.h>

#include <LibCore/UUID.h>
#include <LibScene/Entity.h>
#include <LibScene/Scene.h>
#include <LibScene/Components.h>

#include <gtest/gtest.h>
namespace Terran::World::Tests {

class EntityTest : public testing::Test {

protected:
    EntityTest()
        : m_scene(Core::RefPtr<Scene>::create(m_event_dispatcher))
    {
    }

    virtual ~EntityTest() override = default;

    Core::EventDispatcher m_event_dispatcher;
    Core::RefPtr<Scene> m_scene;
};

TEST_F(EntityTest, id_getter_returns_invalid_if_entity_doesnt_have_tag_component)
{
    Entity entity = m_scene->create_empty_entity();
    ASSERT_FALSE((bool)entity.id());
}

TEST_F(EntityTest, id_getter_returns_valid_if_entity_has_tag_component)
{
    Entity entity = m_scene->create_entity();
    ASSERT_TRUE((bool)entity.id());
}

TEST_F(EntityTest, set_parent_makes_child_report_correct_parent)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    ASSERT_EQ(child.parent(), parent);
}

TEST_F(EntityTest, has_parent_returns_false_when_entity_doesnt_have_relationship_component)
{
    Entity entity = m_scene->create_entity("Parent");
    ASSERT_FALSE(entity.has_parent());
}

TEST_F(EntityTest, entity_reports_correct_scene_id)
{
    Entity entity = m_scene->create_entity("Parent");
    ASSERT_EQ(entity.scene_id(), m_scene->id());
}

TEST_F(EntityTest, set_parent_adds_child_to_parents_children_list)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    ASSERT_EQ(parent.children().size(), 1u);
}

TEST_F(EntityTest, child_at_returns_invalid_when_entity_doesnt_have_relationship_component)
{
    Entity entity = m_scene->create_entity("Parent");
    ASSERT_FALSE((bool)entity.child_at(0));
}

TEST_F(EntityTest, parent_getter_returns_invalid_when_entity_doesnt_have_relationship_component)
{
    Entity entity = m_scene->create_entity("Parent");
    ASSERT_FALSE((bool)entity.parent());
}

TEST_F(EntityTest, child_is_child_of_parent)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    ASSERT_TRUE(child.is_child_of(parent));
}

TEST_F(EntityTest, unparent_removes_child_from_parent)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    child.unparent();
    ASSERT_EQ(parent.children().size(), 0u);
}

TEST_F(EntityTest, unparent_clears_childs_parent_reference)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent);
    child.unparent();
    ASSERT_FALSE(child.has_parent());
}

TEST_F(EntityTest, unparent_returns_error_if_entity_doesnt_have_relationship_component)
{
    Entity child = m_scene->create_entity("Child");
    auto unparentRes = child.unparent();
    ASSERT_FALSE(unparentRes.is_ok());
    ASSERT_EQ(unparentRes.error(), EntityErrors::DoesntHaveRelationshipComponent);
}

TEST_F(EntityTest, unparent_returns_error_if_entity_parent_isnt_found)
{
    Entity child = m_scene->create_entity("Child");
    auto& relationshipComponent = child.add_component<RelationshipComponent>();
    relationshipComponent.Parent = Core::UUID();
    auto unparentRes = child.unparent();
    ASSERT_FALSE(unparentRes.is_ok());
    ASSERT_EQ(unparentRes.error(), EntityErrors::ParentNotFound);
}

// NOTE: This will happen in a very wierd case,
// you'd have to mess with the Relationship Component directly,
// which is not really recommended,
// still though lets check if the logic to deal with it is correct

TEST_F(EntityTest, unparent_returns_error_if_parent_doesnt_contain_child)
{
    Entity parent = m_scene->create_entity("Parent");
    parent.add_component<RelationshipComponent>();
    Entity child = m_scene->create_entity("Child");
    auto& relationshipComponent = child.add_component<RelationshipComponent>();
    relationshipComponent.Parent = parent.id();
    auto unparentRes = child.unparent();
    ASSERT_FALSE(unparentRes.is_ok());
    ASSERT_EQ(unparentRes.error(), EntityErrors::ParentDoesntContainChild);
}

TEST_F(EntityTest, unparent_returns_error_if_parent_doesnt_have_relationship_component)
{
    Entity parent = m_scene->create_entity("Parent");
    Entity child = m_scene->create_entity("Child");
    auto& relationshipComponent = child.add_component<RelationshipComponent>();
    relationshipComponent.Parent = parent.id();
    auto unparentRes = child.unparent();
    ASSERT_FALSE(unparentRes.is_ok());
    ASSERT_EQ(unparentRes.error(), EntityErrors::DoesntHaveRelationshipComponent);
}

TEST_F(EntityTest, set_parent_does_not_create_cycle_when_parent_is_already_child)
{
    Entity a = m_scene->create_entity("A");
    Entity b = m_scene->create_entity("B");
    a.set_parent(b);
    // Attempting to make b a child of a should be a no-op (cycle guard)
    auto parentRes = b.set_parent(a);
    ASSERT_FALSE(b.is_child_of(a));
    ASSERT_FALSE(parentRes.is_ok());
    ASSERT_EQ(parentRes.error(), EntityErrors::TargetParentIsAlreadyAChildOfThisEntity);
}

TEST_F(EntityTest, set_parent_returns_error_if_entity_is_already_a_child_of_this_parent)
{
    Entity a = m_scene->create_entity("A");
    Entity b = m_scene->create_entity("B");
    a.set_parent(b);
    auto parentRes = a.set_parent(b);
    ASSERT_FALSE(parentRes.is_ok());
    ASSERT_EQ(parentRes.error(), EntityErrors::AlreadyAChildOfThisParent);
}

TEST_F(EntityTest, set_parent_unparents_if_entity_alread_has_another_parent)
{
    Entity a = m_scene->create_entity("A");
    Entity b = m_scene->create_entity("B");
    Entity c = m_scene->create_entity("C");
    a.set_parent(b);
    a.set_parent(c);
    ASSERT_TRUE(b.children().empty());
    ASSERT_TRUE(a.is_child_of(c));
}

TEST_F(EntityTest, reparent_moves_child_to_new_parent)
{
    Entity parent1 = m_scene->create_entity("Parent1");
    Entity parent2 = m_scene->create_entity("Parent2");
    Entity child = m_scene->create_entity("Child");
    child.set_parent(parent1);
    child.reparent(parent2);
    ASSERT_TRUE(child.is_child_of(parent2));
    ASSERT_EQ(parent1.children().size(), 0u);
}

}
