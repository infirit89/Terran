#include "trpch.h"

#include "Physics/PhysicsLayerManager.h"

#include "ProjectSerializer.h"

#include "Utils/SerializerUtils.h"

//#include <json.hpp>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iomanip>

namespace TerranEngine
{
    //using json = nlohmann::ordered_json;

    ProjectSerializer::ProjectSerializer(Shared<Project> project)
       : m_Project(project)
    {

    }

    void ProjectSerializer::Serialize() 
    {
        YAML::Emitter out;

        out << YAML::BeginMap;

        out << YAML::Key << "Physics" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Gravity" << YAML::Value << m_Project->m_PhysicsSettings.Gravity;
        out << YAML::Key << "VelocityIterations" << YAML::Value << m_Project->m_PhysicsSettings.VelocityIterations;
        out << YAML::Key << "PositionIterations" << YAML::Value << m_Project->m_PhysicsSettings.PositionIterations;
        out << YAML::Key << "PhysicsTimestep" << m_Project->m_PhysicsSettings.PhysicsTimestep;

        out << YAML::Key << "Layers" << YAML::Value << YAML::BeginSeq;

        auto layers = PhysicsLayerManager::GetLayers();

        for (auto& layer : PhysicsLayerManager::GetLayers())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Layer" << YAML::Value << layer.Name;
            out << YAML::Key << "Mask" << YAML::Value << YAML::Hex << layer.Mask;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;

        out << YAML::EndMap;

        out << YAML::EndMap;

        auto physicsSettingsPath = m_Project->m_ProjectPath / m_ProjectSettingsPath;
        std::ofstream ofs(physicsSettingsPath);

        ofs << out.c_str();
    }

    bool ProjectSerializer::Deserizlize()
    {
        YAML::Node node;

        try 
        {
            auto settingsPath = m_Project->m_ProjectPath / m_ProjectSettingsPath;
            node = YAML::LoadFile(settingsPath.string());
        }
        catch (const YAML::BadFile& e) 
        {
            TR_CORE_ERROR(TR_LOG_CORE, e.what());
            return false;
        }

        try
        {
            auto physicsSettings = node["Physics"];
            m_Project->m_PhysicsSettings.Gravity = physicsSettings["Gravity"].as<glm::vec2>();
            m_Project->m_PhysicsSettings.VelocityIterations = physicsSettings["VelocityIterations"].as<uint32_t>();
            m_Project->m_PhysicsSettings.PositionIterations = physicsSettings["PositionIterations"].as<uint32_t>();
            m_Project->m_PhysicsSettings.PhysicsTimestep = physicsSettings["PhysicsTimestep"].as<float>();

            int i = 0;
            for (const auto& layer : physicsSettings["Layers"]) 
            {
                PhysicsLayer& physicsLayer = PhysicsLayerManager::GetLayer(i);

                physicsLayer.Name = layer["Layer"].as<std::string>();
                physicsLayer.Mask = layer["Mask"].as<uint16_t>();

                i++;
            }

        }
        catch (const YAML::BadSubscript& e)
        {
            TR_CORE_ERROR(TR_LOG_CORE, e.what());
            return false;
        }

        return true;
    }
}

