/**
 * @file Application.h
 * @author George Dautov (georgedautov808@gmail.com)
 * @brief Core application class providing the main entry point and lifecycle management
 * @ingroup LibMain
 *
 * This file contains the Application class which serves as the base class for all client
 * applications using the Terran engine. It manages the application lifecycle, layer stack,
 * and event system integration.
 *
 * @version 2.0
 * @date 2025-10-14
 */
#pragma once

#include "LibCore/Event.h"
#include "LibCore/Layer.h"
#include "LibCore/LayerStack.h"
#include "LibCore/Result.h"
#include <functional>
#include <span>
#include <string_view>
#include <vector>

int main(int argc, char** argv);

namespace Terran {
namespace Main {

/**
 * @class Application
 * @brief Core application class implementing singleton pattern for engine lifecycle management
 * @ingroup LibMain
 *
 * The Application class serves as the main entry point for Terran engine applications.
 * It manages the application's main loop, layer system, and provides event dispatching
 * capabilities. Client applications should inherit from this class to create their
 * custom application logic.
 *
 * Key features:
 * - Singleton pattern ensures only one application instance exists
 * - Layer-based architecture for organizing application logic
 * - Event system integration for handling user input and system events
 * - Main loop management with frame timing
 *
 * @note This class is currently undergoing refactoring as part of the engine
 *       architecture reorganization. Some features like event dispatching
 *       are temporarily disabled.
 *
 * Usage example:
 * @code
 * class MyApp : public Terran::Main::Application {
 * public:
 *     MyApp() {
 *         push_layer(new GameLayer());
 *         push_layer(new UILayer());
 *     }
 * };
 *
 * Terran::Main::Application* create_application(std::span<std::string_view> args) {
 *     return new MyApp();
 * }
 * @endcode
 */
class Application {
public:
    using LifeCycleHookResult = Terran::Core::Result<void>;
    using LifeCycleHook = std::function<LifeCycleHookResult()>;
    /**
     * @brief Adds a layer to the application's layer stack
     *
     * Pushes the specified layer onto the layer stack and immediately calls
     * the layer's OnAttach() method to initialize it. Layers are processed
     * in the order they are added during the update loop.
     *
     * @param layer Pointer to the layer to add. Must not be nullptr.
     * @note The application takes ownership of the layer and will clean it up
     *       during destruction.
     * @see Terran::Core::Layer::OnAttach()
     */
    void push_layer(Terran::Core::Layer* layer);

    void push_startup_hook(LifeCycleHook const& hook)
    {
        m_startupHooks.emplace_back(hook);
    }

    void push_shutdown_hook(LifeCycleHook const& hook)
    {
        m_shutdownHooks.emplace_back(hook);
    }

    /**
     * @brief Initiates application shutdown
     *
     * Signals the application to stop running. The shutdown is not immediate;
     * the application will complete the current frame and exit at the beginning
     * of the next update cycle. This ensures a clean shutdown without interrupting
     * ongoing operations.
     *
     * @note The application will continue processing the current frame after
     *       this call before actually terminating.
     */
    void close();

    /**
     * @brief Dispatches an event to all layers in the layer stack
     *
     * @param event The event to dispatch to layers
     * @warning This method is currently not implemented and may be subject
     *          to changes during the ongoing refactoring.
     * @todo Implement event dispatching system
     */
    void dispatch_event(Terran::Core::Event& event);

    /**
     * @brief Gets the singleton instance of the application
     *
     * Returns the current application instance. The singleton pattern ensures
     * only one application can exist at any given time.
     *
     * @return Pointer to the current application instance, or nullptr if no
     *         application has been created yet
     * @warning Accessing this before an application is created will return nullptr
     */
    static Application* get() { return m_Instance; }

protected:
    /**
     * @brief Constructs the application and sets up the singleton instance
     *
     * Protected constructor enforcing the singleton pattern. Only derived classes
     * can create instances. Asserts that no other application instance exists.
     *
     * @throws Assertion failure if another application instance already exists
     */
    Application() noexcept;

    /**
     * @brief Destroys the application and cleans up all layers
     *
     * Virtual destructor ensures proper cleanup of derived classes.
     * Automatically removes and destroys all layers in the layer stack.
     */
    virtual ~Application();

private:
    /**
     * @brief Executes the main application loop
     *
     * Runs the primary game loop, updating all layers each frame and handling
     * the application lifecycle. Continues until close() is called or an
     * external termination signal is received.
     *
     * @note This method is called by the main() function and should not be
     *       called directly by client code
     * @warning Currently has placeholder timing code and disabled profiling
     */
    [[nodiscard]] Terran::Core::Result<void> run();

    static Application* m_Instance;
    Terran::Core::LayerStack m_Stack;

    bool m_Minimized = false;
    bool m_Running = false;

    friend int ::main(int argc, char** argv);
    std::vector<LifeCycleHook> m_startupHooks;
    std::vector<LifeCycleHook> m_shutdownHooks;
};

} // namespace Main
} // namespace Terran

/**
 * @brief Factory function for creating application instances
 *
 * This function must be implemented by client applications to provide
 * the engine with a way to create their custom application instance.
 * Called by the engine's main() function during startup.
 *
 * @param arguments Command line arguments passed to the application
 * @return Pointer to the created application instance
 *
 * @note The engine takes ownership of the returned application instance
 *       and will handle its destruction.
 *
 * Example implementation:
 * @code
 * Terran::Main::Application* create_application(std::span<std::string_view> arguments) {
 *     return new MyGameApplication();
 * }
 * @endcode
 */
Terran::Main::Application* create_application(std::span<std::string_view> arguments);
