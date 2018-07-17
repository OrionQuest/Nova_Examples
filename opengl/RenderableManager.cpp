#include "RenderableManager.h"
#include "ApplicationFactory.h"
#include <exception>
#include <limits>
#include <iostream>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

Nova::RenderableManager::RenderableManager( ApplicationFactory& app ) : _app(app), _resource_counter(0)
{

}

Nova::RenderableManager::~RenderableManager(){
}

unsigned long
Nova::RenderableManager::MaxId()
{
    return  std::numeric_limits<unsigned long>::max();
}

void
Nova::RenderableManager::AddFactory( std::unique_ptr<RenderableFactory> factory )
{
  _factories.push_back( std::move( factory ) );
}

std::vector<unsigned long>
Nova::RenderableManager::RegisterRenderables( std::string path )
{
    fs::path renderablePath;
    renderablePath /= path;
    try{
        renderablePath = fs::canonical( renderablePath );
    }
    catch( std::exception& e ){
        throw std::runtime_error(std::string("Could not validate Renderable path: ") + e.what() );
    }

    if( renderablePath.has_extension() ){
        std::string ext = renderablePath.extension().native();
        ext = ext.substr(1); // Strip leading '.'
        //std::cout << "Looking for a factory to load a '" << ext << "' file." << std::endl;
        for( auto& factory : _factories ){
            if( factory->AcceptExtension( ext ) ){
                std::vector<unsigned long> renderableIds;
                std::vector< std::unique_ptr<Renderable> > renderables;
                try{
                    // TODO: Remove special case for a single renderable...
                    // This will break old style plugins
                    if( factory->NumRenderables(renderablePath.native()) == 1 ){
                        std::unique_ptr<Renderable> renderable(factory->Create( _app, renderablePath.native()));
                        renderables.push_back( std::move( renderable ) );
                    }
                    else
                        renderables = std::move(factory->CreateMultiple( _app, renderablePath.native()));
                }
                catch( std::exception& e ){
                    std::cout << "Failure: Factory "<< factory->Name() <<" threw an exception when loading renderable: " << e.what() << std::endl;
                }

                for( auto&& renderable : renderables ){
                    if(!renderable)
                        throw std::runtime_error( std::string( "Unable to load renderable. Factory " ) + factory->Name() + std::string(" returned nothing." ) );
                    
                    unsigned long renderableId = _resource_counter++;
                    _resources.insert( std::make_pair( renderableId, std::move( renderable )));
                    renderableIds.push_back( renderableId );
                }                
                return renderableIds;            
            }
        }
        throw std::runtime_error( std::string("Unable to load renderable. No factory available to handle type: ") + ext );
    }
    else{
        throw std::runtime_error( "Unable to load renderable. No identifiable extension." );        
    }
}

Nova::Renderable* 
Nova::RenderableManager::Get( unsigned long id )
{
    auto res = _resources.find( id );
    if( res != _resources.end() ){
      return res->second.get();
    }
    else{
        throw std::runtime_error( "No resource matching the specified id was registered." );
    }
    
}


