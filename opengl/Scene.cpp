#include "Scene.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <utility>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp> 

#include "ApplicationFactory.h"
#include "IOEvent.h"
#include "ViewportManager.h"
#include "Shader.h"

Nova::Scene::Scene(ApplicationFactory& app) : _app(app), _debugHitSelection(false), scene_sphere( glm::vec4(0,0,0,10)), playback_enabled(false), animation_fps(1.0f/(app.GetConfig().fps)), playback_callback(0)
{
    hit_start = glm::vec3(0,0,0);
    hit_end = glm::vec3(1,0,0);
    selected_object = _app.GetRenderableManager().MaxId();
    _app.GetIOService().On( "TOGGLE-DEBUG", [&](IOEvent& event){
            if( event.command_data->args.size() >= 1 &&
                event.command_data->args.at(0) == "SELECTION" )
                _debugHitSelection = !_debugHitSelection;
        });
    _app.GetIOService().On( "TOGGLE-DISPLAY", [&](IOEvent& event){
            if( event.command_data->args.size() == 0 ){
                for( auto& object : _scene_objects ){
                    object->enable_draw = true;
                }
            }

            if( event.command_data->args.size() == 1 ){
                int object_id;
                try{
                    object_id = stoi( event.command_data->args.at(0) );
                    for( auto& object : _scene_objects ){
                        if( object->renderable_id == object_id )
                            object->enable_draw = !object->enable_draw;
                    }
                }
                catch( std::exception& e ){
                    std::cout << "Could not toggle rendering for object '"<< event.command_data->args.at(0)<<"'" << std::endl;
                    return;
                }
            }

            if( event.command_data->args.size() == 2 ){
                int object_id_start, object_id_end;
                try{
                    object_id_start = stoi( event.command_data->args.at(0) );
                    object_id_end = stoi( event.command_data->args.at(1) );
                    for( auto& object : _scene_objects ){
                        if( object->renderable_id >= object_id_start &&
                            object->renderable_id <= object_id_end )
                            object->enable_draw = !object->enable_draw;
                    }
                }
                catch( std::exception& e ){
                    std::cout << "Could not toggle rendering for object range '"<< event.command_data->args.at(0) << " - " << event.command_data->args.at(1) <<"'" << std::endl;
                    return;
                }
            }
        });

    _app.GetIOService().On( "LIST-OBJECTS",  [&](IOEvent& event){
            std::cout << "Scene Objects: " << std::endl;
            for( auto& object : _scene_objects ){
                Renderable* renderable = _app.GetRenderableManager().Get( object->renderable_id );
                std::cout << "( " << object->renderable_id << " ): " <<
                    renderable->render_class() << ":"<< renderable->name() <<
                    " [" << object->src_path << "]" <<  std::endl;
            }
        });
}

std::vector< std::string > 
tokenize( std::string str ){
    typedef std::string::size_type size_type;
    std::vector< std::string > tokens;
    size_type pos = 0;
    while( pos != std::string::npos ){
        size_type next_token = str.find_first_of(".", pos);
        if( next_token != std::string::npos ){
            tokens.push_back( str.substr( pos, next_token-pos ) );
            pos = next_token+1;
        }
        else{
            tokens.push_back( str.substr( pos ) );
            pos = std::string::npos;
        }
    }                         
    return tokens;
}

void
Nova::Scene::Configure( std::string path ){

    for( auto plugin : _app.GetConfig().pluginList ){
      _app.GetPluginManager().LoadPlugin( plugin );
    }    


    _path = path;

    fs::path config_path;
    config_path /= path;
    config_path /= "scene.conf";

    po::options_description config("Configuration");
    config.add_options()
        ;
    
    std::ifstream ifs(config_path.c_str());
    if (!ifs)
        return;

    po::variables_map vm;
    auto results = parse_config_file(ifs, config, true);
    store( results, vm );
    notify(vm);        

}

void
Nova::Scene::Playback( IOEvent& event){
    IOEvent newevent(IOEvent::COMMAND);
    newevent.command_data->command = "NEXT-FRAME";
    newevent.currentTime = event.currentTime;
    _app.GetIOService().Trigger( newevent );        
}

void
Nova::Scene::TogglePlayback(){
    if( playback_callback ){
        _app.GetIOService().ClearTimer( playback_callback );
        playback_callback = 0;
    }
    else
        playback_callback = _app.GetIOService().OnTimer( IOService::Timer(animation_fps, 
                                                                          IOService::Timer::REPEAT | 
                                                                          IOService::Timer::DEBOUNCE ),
                                                         [&](IOEvent& event){ this->Playback(event); });
}

void
Nova::Scene::StopPlayback(){
    _app.GetIOService().ClearTimer( playback_callback );
    playback_callback = 0;
}


void
Nova::Scene::Load()
{

    fs::path load_path;
    load_path /= _path;    
    //load_path = fs::canonical(load_path);
    //std::cout << "Loading " << load_path.native() << std::endl;
    load_path /= "./";

    fs::directory_iterator diter( load_path );
    for( ; diter != fs::directory_iterator(); diter++){
        std::vector<unsigned long> objectIds;
        try{
            objectIds = _app.GetRenderableManager().RegisterRenderables( diter->path().native() );
        }
        catch( std::exception& e ){
            // We failed to load this particular file, say something about it...
            //std::cout << "Couldn't load " << diter->path().native() << ", Reason: " << std::endl;
            //std::cout << e.what() << std::endl;
        }
        for( auto objectId : objectIds ){           
            std::unique_ptr<SceneObject> object( new SceneObject );
            object->src_path = diter->path().native();
            object->renderable_id = objectId;
            object->enable_draw = true;
            _scene_objects.push_back( std::move( object ) );
            std::cout << "Loaded " << diter->path().filename() << " with id " << objectId << std::endl;
        }
    }    
    
    InitializeSliceParameters();
    BuildBoundingSphere();
}

void 
Nova::Scene::InitializeSliceParameters(){

    bool slice_rdy = false;
    glm::vec3 slice_start;
    float dx;
    glm::vec3 max_slice;

    for( auto& object : _scene_objects ){
        Renderable* renderable = _app.GetRenderableManager().Get( object->renderable_id );
        if( renderable->slice_provider() ){
            slice_start = renderable->slice_start();
            dx = renderable->slice_dx();
            max_slice = renderable->max_slice();
            slice_rdy = true;
            std::cout << "Using Object " << object->renderable_id << " as the slice provider" << std::endl;
            break;
        }
    }
    if( slice_rdy )
        _app.GetWorld().GetViewportManager().Configure_SliceMode(slice_start, dx, max_slice);
    else
        std::cout << "Could not locate a slice provider. Slice mode is disabled." << std::endl;
}

glm::vec4
Nova::Scene::BoundingSphere(){
    return scene_sphere;
}

void
Nova::Scene::BuildBoundingSphere(){
    std::vector<glm::vec3> canidate_points;
    for( auto& object : _scene_objects ){
        Renderable* renderable = _app.GetRenderableManager().Get( object->renderable_id );
        glm::vec4 sphere = renderable->bounding_sphere();
        if( sphere.w < 0 )
            continue;
        canidate_points.push_back( glm::vec3( sphere ) );
        canidate_points.push_back( glm::vec3( sphere ) + glm::vec3( sphere.w, 0, 0) );
        canidate_points.push_back( glm::vec3( sphere ) - glm::vec3( sphere.w, 0, 0) );
        canidate_points.push_back( glm::vec3( sphere ) + glm::vec3( 0, sphere.w, 0) );
        canidate_points.push_back( glm::vec3( sphere ) - glm::vec3( 0, sphere.w, 0) );
        canidate_points.push_back( glm::vec3( sphere ) + glm::vec3( 0, 0, sphere.w) );
        canidate_points.push_back( glm::vec3( sphere ) - glm::vec3( 0, 0, sphere.w) );
    }
    if( canidate_points.size() == 0 ){
        scene_sphere = glm::vec4(0,0,0,10);
        return;
    }
    
    // Use Ritter's bounding sphere algorithm to build sphere
    glm::vec3 initial_point = canidate_points.at(0);
    glm::vec3 farthest_point1 = initial_point;
    for( auto p : canidate_points ){
        if( glm::distance( initial_point, farthest_point1 ) < glm::distance( initial_point, p ) )
            farthest_point1 = p;
    }
    glm::vec3 farthest_point2 = farthest_point1;
    for( auto p : canidate_points ){
        if( glm::distance( farthest_point1, farthest_point2 ) < glm::distance( farthest_point1, p ) )
            farthest_point2 = p;
    }
    glm::vec3 midpoint = glm::mix(farthest_point1, farthest_point2, 0.5f);
    float radius = glm::distance( farthest_point1, farthest_point2) / 2;
    for( auto p : canidate_points ){
        if( glm::distance( midpoint, p ) > radius )
            radius = glm::distance( midpoint, p );
    }
    scene_sphere.x = midpoint.x;
    scene_sphere.y = midpoint.y;
    scene_sphere.z = midpoint.z;
    scene_sphere.w = radius;
    std::cout << "Bounding Sphere is centered at  " << scene_sphere.x << " " << 
        scene_sphere.y << " " << 
         scene_sphere.z << ", and has a radius of " << 
        scene_sphere.w << std::endl;
}

void Nova::Scene::Draw()
{
    if( _debugHitSelection ){
        GLfloat vertices[6];
        vertices[0] = hit_start.x;
        vertices[1] = hit_start.y;
        vertices[2] = hit_start.z;
        vertices[3] = hit_end.x;
        vertices[4] = hit_end.y;
        vertices[5] = hit_end.z;
        
        GLuint VBO;
        GLuint VAO;
        glGenBuffers(1, &VBO); 
        glGenVertexArrays(1, &VAO);  
        
        glBindVertexArray(VAO);
        // 2. Copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        // 3. Then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);  
        
        glm::mat4 projection,view,model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();
        auto lineShader = _app.GetShaderManager().GetShader("BasicLineShader");
        lineShader->SetMatrix4("projection",projection);
        lineShader->SetMatrix4("view",view);
        lineShader->SetMatrix4("model",model);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);      
    }

    for( auto& object : _scene_objects ){
        if( object->enable_draw ){
            Renderable* renderable = _app.GetRenderableManager().Get( object->renderable_id );
            renderable->draw();
        }
    }
}


void 
Nova::Scene::Selection( IOEvent& event )
{
    if( event.mousebutton_data->button != IOEvent::M_LEFT || !(event.mousebutton_data->mods & IOEvent::mSHIFT) )
        return;

    // Build Hit Test line

    glm::vec3 mouse_position;
    glm::mat4 view = _app.GetWorld().Get_ViewMatrix(World::INTERACTION);
    glm::mat4 model = _app.GetWorld().Get_ModelMatrix(World::INTERACTION);
    glm::mat4 projection = _app.GetWorld().Get_ProjectionMatrix(World::INTERACTION);
    glm::vec4 viewport = _app.GetWorld().Get_Viewport(World::INTERACTION);

    mouse_position.x = event.mousebutton_data->x;
    mouse_position.y = (viewport[3] - event.mousebutton_data->y); // Fix the Y direction

    mouse_position.z = 0.0;
    hit_start = glm::unProject( mouse_position, view*model, projection, viewport );
    mouse_position.z = 1.0;
    hit_end = glm::unProject( mouse_position, view*model, projection, viewport );

    //std::cout << "Mouse Point: " << glm::to_string(mouse_position) << std::endl;
    //std::cout << "ViewPort" << glm::to_string(viewport) << std::endl;
    //std::cout << "Hit line : " << glm::to_string(hit_start) << " " << glm::to_string(hit_end) << std::endl;

    if( selected_object != _app.GetRenderableManager().MaxId() )
        _app.GetRenderableManager().Get( selected_object ) -> unassign_selection();
    
    float min_distance = std::numeric_limits<float>::infinity();
    selected_object = _app.GetRenderableManager().MaxId();
    for( auto& object : _scene_objects ){
        Renderable* renderable = _app.GetRenderableManager().Get( object->renderable_id );
        
        // If we are selectable, run the hit tests now!
        if( renderable->selectable() ){
            // First prune with line-sphere intersection test
            glm::vec4 sphere = renderable->bounding_sphere();
            if( sphere.w <= 0 )
                sphere.w = 0.0f;
            glm::vec3 sphere_insec_p1, sphere_insec_p2;
            glm::vec3 sphere_insec_n1, sphere_insec_n2;
            bool potentially_intersects = glm::intersectLineSphere( hit_start,
                                                                    hit_end,
                                                                    glm::vec3(sphere.x,sphere.y,sphere.z),
                                                                    sphere[3],
                                                                    sphere_insec_p1,
                                                                    sphere_insec_n1,
                                                                    sphere_insec_p2,
                                                                    sphere_insec_n2);
            if( potentially_intersects ){
                // Now do the real intersection test which might be very expensive
                float distance = renderable->hit_test( hit_start, hit_end );
                if( distance > 0.0f && distance < min_distance ){
                    min_distance = distance;
                    selected_object = object->renderable_id;
                }
            }
        }
    }
    if( selected_object != _app.GetRenderableManager().MaxId() ){
        _app.GetRenderableManager().Get( selected_object ) -> assign_selection( hit_start,
                                                                                hit_end,
                                                                                glm::normalize( hit_end-hit_start )*min_distance + hit_start );       
    }

}
