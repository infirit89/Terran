#include "trpch.h"

#include "Physics/PhysicsLayerManager.h"

#include "ProjectSerializer.h"

#include "Utils/SerializerUtils.h"

#include <json.hpp>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iomanip>

namespace TerranEngine
{
    using json = nlohmann::ordered_json;

    ProjectSerializer::ProjectSerializer(Shared<Project>& project)
       : m_Project(project)
    {

    }

    void ProjectSerializer::Serialize() 
    {
        SerializePhysicsSettings();
    }

    bool ProjectSerializer::Deserizlize()
    {
        bool result = DeserializePhysicsSettings();
        return result;
    }

    void ProjectSerializer::SerializePhysicsSettings()
    {
        YAML::Emitter out;

        json j;
        
        out << YAML::BeginMap;
        
        out << YAML::Key << "Gravity" << YAML::Value << m_Project->PhysicsSettings.Gravity;
        out << YAML::Key << "VelocityIterations" << YAML::Value << m_Project->PhysicsSettings.VelocityIterations;
        out << YAML::Key << "PositionIterations" << YAML::Value << m_Project->PhysicsSettings.PositionIterations;
        out << YAML::Key << "PhysicsTimestep" << m_Project->PhysicsSettings.PhysicsTimestep;

        out << YAML::Key << "Layers" << YAML::Value << YAML::BeginSeq;

        auto layers = PhysicsLayerManager::GetLayers();

        for(auto& layer : PhysicsLayerManager::GetLayers())
        {
            out << YAML::BeginMap;
            out << YAML::Key << layer.Name << YAML::Value << YAML::Hex << layer.Mask;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;

        out << YAML::EndMap;

        auto physicsSettingsPath = m_Project->m_ProjectPath / m_ProjectSettingsPath / m_PhysicsSettingsFile;
        std::ofstream ofs(physicsSettingsPath);

        ofs << out.c_str();
    }

    bool ProjectSerializer::DeserializePhysicsSettings()
    {
        auto physicsSettingsPath = m_Project->ProjectPath / m_ProjectSettingsPath / m_PhysicsSettingsFile;
        std::ifstream ifs(physicsSettingsPath);

        try 
        {
            json j;
            ifs >> j;

            m_Project->Settings.Gravity = SerializerUtils::DeserializeVec2(j, "Gravity");
            m_Project->Settings.VelocityIterations = j["VelocityIterations"];
            m_Project->Settings.PositionIterations = j["PositionIterations"];
            m_Project->Settings.PhysicsTimestep = j["PhysicsTimestep"];

            int i = 0;
            for (json jLayer : j["Layers"]) 
            {
                PhysicsLayer& layer = PhysicsLayerManager::GetLayer(i);
                std::string layerName = jLayer.begin().key();
                uint16_t layerMask = jLayer[layerName];

                layer.Name = layerName;
                layer.Mask = layerMask;

                i++;
            }
        }
        catch (const std::exception& e) 
        {
            TR_CLIENT_ERROR(e.what());
            return false;
        }

        return true;
    }
}

