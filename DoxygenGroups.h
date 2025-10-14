/**
 * @file DoxygenGroups.h
 * @brief Doxygen group definitions for organizing the Terran engine documentation
 * 
 * This file contains all the main group definitions used to organize the Terran engine
 * documentation into logical modules and packages. Include this file in your Doxygen
 * configuration to ensure proper group hierarchy.
 *
 * @author George Dautov (georgedautov808@gmail.com)
 * @version 2.0
 * @date 2025-10-14
 */

/**
 * @defgroup LibMain Application Framework
 * @brief Core application lifecycle and entry point management
 * 
 * The LibMain module provides the fundamental application framework for Terran engine
 * applications, including the main application class, entry point management, and
 * core lifecycle functionality. This module serves as the bridge between client
 * applications and the engine core.
 * 
 * Key components:
 * - Application class for lifecycle management
 * - Entry point handling and initialization
 * - Command line argument processing
 * - Application factory pattern implementation
 */

/**
 * @defgroup LibCore Core Utilities
 * @brief Fundamental utilities and systems shared across the engine
 * 
 * The LibCore module contains essential utilities, data structures, and systems
 * that are used throughout the Terran engine. This includes event handling, layer
 * management, time utilities, and core abstractions that provide the foundation
 * for all other engine systems.
 * 
 * Key components:
 * - Event system and dispatching
 * - Layer management and layer stack
 * - Time and timing utilities
 * - Core data structures and algorithms
 * - Assertion and debugging utilities
 */

/**
 * @defgroup TerranEngine Engine Systems
 * @brief Core engine functionality and rendering systems
 * 
 * The TerranEngine module contains the main engine systems including graphics
 * rendering, physics integration, asset management, and platform-specific code.
 * This module provides the core functionality that powers Terran engine applications.
 * 
 * Key components:
 * - 2D graphics rendering system
 * - Physics integration (Box2D)
 * - Asset management and loading
 * - Platform abstraction layer
 * - Input handling and window management
 */

/**
 * @defgroup LibCore_Events Event System
 * @ingroup LibCore
 * @brief Event handling and dispatching system
 * 
 * The event system provides a flexible way to handle user input, system events,
 * and inter-component communication throughout the engine.
 */

/**
 * @defgroup LibCore_Layers Layer Management
 * @ingroup LibCore
 * @brief Layer stack and layer lifecycle management
 * 
 * The layer system allows organizing application logic into discrete, manageable
 * layers that can be updated and rendered independently.
 */

/**
 * @defgroup LibCore_Time Time and Timing
 * @ingroup LibCore
 * @brief Time utilities and frame timing management
 * 
 * Time utilities for managing frame timing, delta time calculations, and
 * time-based operations throughout the engine.
 */

/**
 * @defgroup TerranEngine_Graphics Graphics System
 * @ingroup TerranEngine
 * @brief 2D graphics rendering and batch processing
 * 
 * The graphics system provides 2D rendering capabilities with batch processing
 * for optimal performance, including sprite rendering, text rendering, and
 * shader management.
 */

/**
 * @defgroup TerranEngine_Physics Physics System
 * @ingroup TerranEngine
 * @brief 2D physics simulation and collision detection
 * 
 * Physics system integration with Box2D for 2D physics simulation, collision
 * detection, and rigid body dynamics.
 */

/**
 * @defgroup TerranEngine_Assets Asset System
 * @ingroup TerranEngine
 * @brief Asset loading, management, and caching
 * 
 * Asset management system for loading, caching, and managing game assets
 * including textures, sounds, shaders, and other resources.
 */