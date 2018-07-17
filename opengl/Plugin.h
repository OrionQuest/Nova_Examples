///////////////////////////////////////////////////////////////////////////////
// Plugin architecture example                                               //
//                                                                           //
// This code serves as an example to the plugin architecture discussed in    //
// the article and can be freely used                                        //
///////////////////////////////////////////////////////////////////////////////
#ifndef OPENGL_3D_PLUGIN_H
#define OPENGL_3D_PLUGIN_H

#include "SharedLibrary.h"

#include <string>

namespace Nova {

  // ----------------------------------------------------------------------- //

  class ApplicationFactory;

  // ----------------------------------------------------------------------- //

  /// Representation of a plugin
  class Plugin {

    /// <summary>Signature for the version query function</summary>
    private: typedef int GetEngineVersionFunction();
    /// <summary>Signature for the plugin's registration function</summary>
    private: typedef void RegisterPluginFunction(ApplicationFactory &);

    /// <summary>Initializes and loads a plugin</summary>
    /// <param name="filename">Filename of the plugin to load</summary>
    public: Plugin(const std::string &filename);
    /// <summary>Copies an existing plugin instance</summary>
    public: Plugin(const Plugin &other);
    /// <summary>Unloads the plugin</summary>
    public: ~Plugin();

    /// <summary>Queries the plugin for its expected engine version</summary>
    public: int getEngineVersion() const {
      return this->getEngineVersionAddress();
    }

    public: static std::string DefaultExtension() {
          return SharedLibrary::DefaultExtension();
    }

    /// <summary>Register the plugin to a applicationfactory</summary>
    /// <param name="applicationfactory">Applicationfactory the plugin should register to</param>
    public: void registerPlugin(ApplicationFactory &app) {
      this->registerPluginAddress(app);
    }

    /// <summary>Creates a copy of the plugin instance</summary>
    public: Plugin &operator =(const Plugin &other);

    /// <summary>Handle of the loaded shared library</summary>
    private: SharedLibrary::HandleType sharedLibraryHandle;
    /// <summary>Number of references that exist to the shared library</summary>
    private: size_t *referenceCount;
    /// <summary>Function to query for the expected engine version</summary>
    private: GetEngineVersionFunction *getEngineVersionAddress;
    /// <summary>Registers the plugin with the ApplicationFactory</summary>
    private: RegisterPluginFunction *registerPluginAddress;

  };

  // ----------------------------------------------------------------------- //

} // namespace Nova

#endif // OPENGL_3D_PLUGIN_H
