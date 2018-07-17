#include "ResourceManager.h"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <iostream>

std::string
Nova::AbstractResourceManager::GetPath( std::string basefile ){
    // Before starting, check for absolute paths...
    {
        fs::path test_path;
        test_path /= basefile;
        if( test_path.is_absolute() ){
            fs::file_status status = fs::status( test_path );
            if( fs::exists( status ) )
                return std::string(test_path.native());
        }
    }

    for( auto search_path : _search_paths ){
        fs::path test_path;
        test_path /= search_path;
        test_path /= basefile;
        fs::file_status status = fs::status( test_path );
        if( fs::exists( status ) )
            return std::string(test_path.native());
    }  
    return std::string();
}


std::vector<std::string>
Nova::AbstractResourceManager::GetAllPaths( std::string basefile ){
    std::vector< std::string > potential_paths;
    // Before starting, check for absolute paths...
    {
        fs::path test_path;
        test_path /= basefile;
        if( test_path.is_absolute() ){
            fs::file_status status = fs::status( test_path );
            if( fs::exists( status ) )
                potential_paths.push_back(std::string(test_path.native()));
        }
    }

    for( auto search_path : _search_paths ){
        fs::path test_path;
        test_path /= search_path;
        test_path /= basefile;
        fs::file_status status = fs::status( test_path );
        if( fs::exists( status ) )
            potential_paths.push_back( std::string(test_path.native()) );
    }
    return potential_paths;
}
