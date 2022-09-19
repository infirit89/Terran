#include "trpch.h"

#include "Physics/PhysicsLayerManager.h"

#include "ProjectSerializer.h"

#include "Utils/SerializerUtils.h"

#include <json.hpp>

#include <fstream>
#include <iomanip>

namespace TerranEngine
{
    using json = nlohmann::ordered_json;

    ProjectSerialzer::ProjectSerialzer(Shared<Project>& project)
       : m_Project(project)
    {

    }

    void ProjectSerialzer::SerializePhysicsSettings()
    {
        json j;
        
        j["Gravity"] = SerializerUtils::SerializeVec2(m_Project->Settings.Gravity);
        j["VelocityIterations"] = m_Project->Settings.VelocityIterations;
        j["PositionIterations"] = m_Project->Settings.PositionIterations;
        j["PhysicsTimestep"] = m_Project->Settings.PhysicsTimestep;

        auto layers = PhysicsLayerManager::GetLayers();
        j["Layers"] = { };
        json& jLayers = j["Layers"];

        for(auto& layer : PhysicsLayerManager::GetLayers())
        {
            json jLayer = { { layer.Name, layer.Mask } };
            jLayers.push_back(jLayer);
        }

        auto physicsSettingsPath = m_Project->ProjectPath / m_ProjectSettingsPath / m_PhysicsSettingsFile;
        std::ofstream ofs(physicsSettingsPath);

        ofs << j.dump(4) << std::endl;
    }

    bool ProjectSerialzer::DeserializePhysicsSettings()
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

