#ifndef __OPENGL_3D_ABSTRACT_RESOURCE_MANAGER_H__
#define __OPENGL_3D_ABSTRACT_RESOURCE_MANAGER_H__

#include <vector>
#include <string>

namespace Nova{
    
    class AbstractResourceManager {
    public:
        AbstractResourceManager() {};
        virtual ~AbstractResourceManager() {};
        
        virtual void ClearSearchPaths() {
            _search_paths.clear();
        }

        virtual void SetSearchPaths( const std::vector< std::string >& paths ) {
            _search_paths = paths;
        }

        virtual void PrependSearchPaths( const std::vector< std::string >& paths ) {
            _search_paths.insert( _search_paths.begin(), paths.begin(), paths.end() );
        }

        virtual void PrependSearchPaths( const std::string path ) {
              _search_paths.insert( _search_paths.begin(), path );
        }

        virtual void AppendSearchPaths( const std::vector< std::string >& paths ) {
              _search_paths.insert( _search_paths.end(), paths.begin(), paths.end() );
        }

        virtual void AppendSearchPaths( const std::string path ) {
              _search_paths.insert( _search_paths.end(), path );
        }

        virtual std::vector<std::string> GetSearchPaths() {
            return _search_paths;
        }

        virtual std::string GetPath( std::string basefile );
        virtual std::vector<std::string> GetAllPaths( std::string basefile );


    protected:
        std::vector< std::string > _search_paths;
    };

}

#endif
