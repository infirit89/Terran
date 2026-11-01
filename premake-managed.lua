workspace "Terran-Managed"

configurations {
    "Debug",
    "Release",
}

multiprocessorcompile "On"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "vendor/Coral/Coral.Managed"

group "Core"
include "Terran.Core"
