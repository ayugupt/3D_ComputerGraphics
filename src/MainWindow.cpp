#include "MainWindow.h"
#include<iostream>
#include<glibmm/refptr.h>
#include<string>
#include<cmath>
#include "MatrixFunctions.h"
#include"Camera.h"
#include<map>
#include<fstream>
#include<gtkmm/menubar.h>
#include<gtkmm/filechooserdialog.h>
#include<tinyexpr.h>

MainWindow::MainWindow(): m_body_box{Gtk::ORIENTATION_VERTICAL}, m_camera_position_box{Gtk::ORIENTATION_VERTICAL, 8},
                         m_camera_position_x_adjustment{Gtk::Adjustment::create(0, -100, 100)}, m_camera_position_y_adjustment{Gtk::Adjustment::create(0, -100, 100)}, m_camera_position_z_adjustment{Gtk::Adjustment::create(10, -100, 100)},
                         m_camera_position_x{m_camera_position_x_adjustment}, m_camera_position_y{m_camera_position_y_adjustment}, m_camera_position_z{m_camera_position_z_adjustment},
                         m_builder{Gtk::Builder::create()}, m_simple_action_group{Gio::SimpleActionGroup::create()},
                         m_function_entry_buffer{Gtk::EntryBuffer::create()}, m_confirm_function{"Generate"}{

    RenderMenu();
    RenderOverlay();
    RenderCameraControl();

    set_default_size(800, 600);
    add(m_body_box);
    m_body_box.show();

    m_camera.SetGlobalPosition(Coords{m_camera_position_x.get_value(), m_camera_position_y.get_value(), m_camera_position_z.get_value()});
}

MainWindow::MainWindow(const char *fileName): MainWindow::MainWindow(){
    LoadModel(fileName);
}

MainWindow::~MainWindow(){
    if(m_function_expression_ptr != nullptr){
        te_free(m_function_expression_ptr);
        m_function_expression_ptr = nullptr;
    }
}

//---------------------Signal Handlers------------------------------------

bool MainWindow::DrawHandler(const Cairo::RefPtr<Cairo::Context> &cr){
    if(m_model.GetIsLoaded()){
        cr->set_line_width(2);
        cr->set_source_rgb(0, 0, 0);

        Gtk::Allocation allocation = canvas.get_allocation();
        auto height = allocation.get_height();
        auto width = allocation.get_width();

        cr->rectangle(0, 0, width, height);
        cr->fill();

        Coords cameraGlobalPosition = m_camera.GetGlobalPosition();

        std::multimap<double, int> dotResult;

        for(int i = 0; i < m_model.faces.size(); i++){
            Coords directionVector;
            for(auto &vertexNormal:m_model.faces[i].verticesNormal){
                directionVector.x += vertexNormal.coords.x;
                directionVector.y += vertexNormal.coords.y;
                directionVector.z += vertexNormal.coords.z;
            }

            double result = (cameraGlobalPosition.x*directionVector.x + cameraGlobalPosition.y*directionVector.y + cameraGlobalPosition.z*directionVector.z)/(sqrt(cameraGlobalPosition.x*cameraGlobalPosition.x + cameraGlobalPosition.y*cameraGlobalPosition.y + cameraGlobalPosition.z*cameraGlobalPosition.z)*sqrt(directionVector.x*directionVector.x + directionVector.y*directionVector.y + directionVector.z*directionVector.z));
            dotResult.insert({result, i});
        }

        double focalLength = m_camera.GetFocalLength();


        if(m_function_expression_ptr == nullptr){
            for(auto &pair:dotResult){

                cr->set_source_rgb((1+pair.first)/2, 0, 0);
                cr->begin_new_path();

                bool first = true;
                for(auto &vertex:m_model.faces[pair.second].vertices){
                    Matrix vertexMatrix{ {0, {{0, vertex.coords.x}}}, {1, {{0, vertex.coords.y}}}, {2, {{0, vertex.coords.z}}}, {3, {{0, 1}}} };

                    Matrix transformedPoint = MatrixMultiply(m_camera.GetTransformationMatrixGlobalToCameraCoords(), vertexMatrix);

                    Vertex vertexOnImagePlane{-1*m_camera.GetFocalLength()*transformedPoint[0][0]/transformedPoint[2][0], -1*m_camera.GetFocalLength()*transformedPoint[1][0]/transformedPoint[2][0], -1*m_camera.GetFocalLength()};

                    if(first){
                        cr->move_to(width/2 + m_pixels_per_unit*vertexOnImagePlane.coords.x, height/2 - m_pixels_per_unit*vertexOnImagePlane.coords.y);
                    }else{
                        cr->line_to(width/2 + m_pixels_per_unit*vertexOnImagePlane.coords.x, height/2 - m_pixels_per_unit*vertexOnImagePlane.coords.y);
                    }
                    first = false;
                }
                cr->close_path();
                cr->fill();
            }
        }else{
            for(auto &pair:dotResult){
                cr->set_source_rgb((1+pair.first)/2, 0, 0);
                cr->begin_new_path();

                bool first = true;

                Vertex prevSurfacePointProjection;
                Vertex firstSurfacePointProjection;

                for(auto &vertex:m_model.faces[pair.second].vertices){
                    Matrix vertexMatrix{ {0, {{0, vertex.coords.x}}}, {1, {{0, vertex.coords.y}}}, {2, {{0, vertex.coords.z}}}, {3, {{0, 1}}} };

                    Matrix transformedPoint = MatrixMultiply(m_camera.GetTransformationMatrixGlobalToCameraCoords(), vertexMatrix);

                    Vertex surfacePoint{transformedPoint[0][0], transformedPoint[1][0], transformedPoint[2][0]+focalLength};
                    Vertex surfaceDirectionVector{-1*surfacePoint.coords.x, -1*surfacePoint.coords.y, focalLength-surfacePoint.coords.z};

                    double parameter = (-1-surfacePoint.coords.z)/surfaceDirectionVector.coords.z;

                    Vertex surfacePointProjection{surfacePoint.coords.x + parameter*surfaceDirectionVector.coords.x,
                                                 surfacePoint.coords.y + parameter*surfaceDirectionVector.coords.y,
                                                 -1};

                    m_x = surfacePointProjection.coords.x;
                    m_y = surfacePointProjection.coords.y;
                    double zValue = te_eval(m_function_expression_ptr);
                    

                    while(surfacePointProjection.coords.z < zValue && surfacePointProjection.coords.z < 1){
                        surfacePointProjection.coords.z += 0.1;
                        parameter = (surfacePointProjection.coords.z-surfacePoint.coords.z)/surfaceDirectionVector.coords.z;

                        surfacePointProjection.coords.x = surfacePoint.coords.x + parameter*surfaceDirectionVector.coords.x;
                        surfacePointProjection.coords.y = surfacePoint.coords.y + parameter*surfaceDirectionVector.coords.y;

                        m_x = surfacePointProjection.coords.x;
                        m_y = surfacePointProjection.coords.y;

                        zValue = te_eval(m_function_expression_ptr);
                    }

                    if(first){
                        cr->move_to(width/2 -m_pixels_per_unit*(focalLength-2)*surfacePointProjection.coords.x/(surfacePointProjection.coords.z-focalLength), height/2+m_pixels_per_unit*(focalLength-2)*surfacePointProjection.coords.y/(surfacePointProjection.coords.z-focalLength));
                        firstSurfacePointProjection = surfacePointProjection;
                    }else{
                        double changeX = (surfacePointProjection.coords.x-prevSurfacePointProjection.coords.x)/100;
                        double changeY = (surfacePointProjection.coords.y-prevSurfacePointProjection.coords.y)/100;

                        for(int i = 1; i <= 100; i++){
                            m_x = prevSurfacePointProjection.coords.x+i*changeX;
                            m_y = prevSurfacePointProjection.coords.y+i*changeY;
                            double z = te_eval(m_function_expression_ptr);
                            cr->line_to(width/2 -m_pixels_per_unit*(focalLength-2)*(m_x)/(z-focalLength), height/2 + m_pixels_per_unit*(focalLength-2)*(m_y)/(z-focalLength));
                        }
                    }

                    prevSurfacePointProjection = surfacePointProjection;
                    first = false;
                }

                double changeX = (firstSurfacePointProjection.coords.x-prevSurfacePointProjection.coords.x)/100;
                double changeY = (firstSurfacePointProjection.coords.y-prevSurfacePointProjection.coords.y)/100;

                for(int i = 1; i <= 100; i++){
                    m_x = prevSurfacePointProjection.coords.x+i*changeX;
                    m_y = prevSurfacePointProjection.coords.y+i*changeY;
                    double z = te_eval(m_function_expression_ptr);
                    cr->line_to(width/2 - m_pixels_per_unit*(focalLength-2)*(m_x)/(z-focalLength), height/2 + m_pixels_per_unit*(focalLength-2)*(m_y)/(z-focalLength));
                }

                cr->close_path();
                cr->fill();
            }
        }

    }

    return true;
}


void MainWindow::CameraPositionChanged(){
    m_camera.SetGlobalPosition(Coords{m_camera_position_x.get_value(), m_camera_position_y.get_value(), m_camera_position_z.get_value()});
    canvas.queue_draw();
}

// void MainWindow::OverlayFrameSizeAllocateHandler(Gtk::Allocation &allocation){
//     auto width = allocation.get_width();
//     std::cout<<"fw"<<std::endl;

// }

void MainWindow::FunctionButtonHandler(){
    Glib::ustring expression_string = m_function_entry_buffer->get_text();
    if(expression_string.length() > 0){
        if(m_function_expression_ptr != nullptr){
            te_free(m_function_expression_ptr);
        }

        te_variable variables[] = {{"x", &m_x}, {"y", &m_y}};

        m_function_expression_ptr = te_compile(expression_string.c_str(), variables, 2, &m_expression_error);
        // if(m_expression_error == 0){
        //     std::cout<<"Wrong expression"<<std::endl;
        // }
        // m_x = 10;
        // m_y = 20;
        // std::cout<<te_eval(m_function_expression_ptr)<<std::endl;
    }else{
        m_function_expression_ptr = nullptr;
    }
    canvas.queue_draw();
}

//------------------------------------------------------------------------------------

//--------------------------Menu Actions--------------------------------------------

void MainWindow::OpenModel(){
    Gtk::FileChooserDialog dialog{"Please choode an obj file", Gtk::FILE_CHOOSER_ACTION_OPEN};
    dialog.set_transient_for(*this);
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    int result = dialog.run();

    if(result == Gtk::RESPONSE_OK){
        LoadModel(dialog.get_filename().c_str());
        canvas.queue_draw();
    }
}

//-------------------------------------------------------------------------------

//----------------------------Render Functions----------------------------------

void MainWindow::RenderMenu(){
    GetMenuXMLString("/home/ayush/Documents/cpp_projects/3d_viewer/assets/menuxml.xml");

    m_simple_action_group->add_action("open", sigc::mem_fun(*this, &MainWindow::OpenModel));
    insert_action_group("menu", m_simple_action_group);

    m_builder->add_from_string(m_menu_xml);
    auto object = m_builder->get_object("menubar");
    auto gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
    auto pMenuBar = Gtk::make_managed<Gtk::MenuBar>(gmenu);

    m_body_box.pack_start(*pMenuBar, Gtk::PACK_SHRINK);
    pMenuBar->show();
}

void MainWindow::RenderOverlay(){
    canvas.signal_draw().connect(sigc::mem_fun(*this, &MainWindow::DrawHandler), false);
    m_canvas_overlay.add(canvas);

    m_function_entry.set_buffer(m_function_entry_buffer);

    m_confirm_function.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::FunctionButtonHandler));

    m_surface_function_box.pack_start(m_function_entry, Gtk::PACK_SHRINK);
    m_surface_function_box.pack_start(m_confirm_function, Gtk::PACK_SHRINK);
    m_surface_function_frame.add(m_surface_function_box);

    m_surface_function_frame.property_halign().set_value(Gtk::ALIGN_START);
    m_surface_function_frame.property_valign().set_value(Gtk::ALIGN_START);

    m_canvas_overlay.add_overlay(m_surface_function_frame);

    m_body_box.pack_start(m_canvas_overlay);

    // m_surface_function_frame.signal_size_allocate().connect(sigc::mem_fun(*this, &MainWindow::OverlayFrameSizeAllocateHandler));

    m_canvas_overlay.show_all();
}

void MainWindow::RenderCameraControl(){
    m_body_box.pack_start(m_camera_position_box, Gtk::PACK_SHRINK);

    m_camera_position_x_wrapper.set(0.5, 0.5, 0.5, 1.0);
    m_camera_position_y_wrapper.set(0.5, 0.5, 0.5, 1.0);
    m_camera_position_z_wrapper.set(0.5, 0.5, 0.5, 1.0);

    m_camera_position_x_wrapper.add(m_camera_position_x);
    m_camera_position_y_wrapper.add(m_camera_position_y);
    m_camera_position_z_wrapper.add(m_camera_position_z);

    m_camera_position_box.pack_start(m_camera_position_x_wrapper, Gtk::PACK_SHRINK);
    m_camera_position_box.pack_start(m_camera_position_y_wrapper, Gtk::PACK_SHRINK);
    m_camera_position_box.pack_start(m_camera_position_z_wrapper, Gtk::PACK_SHRINK);

    m_camera_position_x.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::CameraPositionChanged));
    m_camera_position_y.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::CameraPositionChanged));
    m_camera_position_z.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::CameraPositionChanged));

    m_camera_position_box.show_all();
}

//-------------------------------------------------------------------------------

void MainWindow::GetMenuXMLString(const char *file_location){
    std::ifstream menu_xml_file{"/home/ayush/Documents/cpp_projects/3d_viewer/assets/menuxml.xml"};
    if(!menu_xml_file){
        throw "Menu file doesn't exist";
    }   

    std::string str;
    while(std::getline(menu_xml_file, str)){
        m_menu_xml = m_menu_xml+str;
    }

    menu_xml_file.close();
}

void MainWindow::LoadModel(const char* file_path){
    m_model.LoadObjModel(file_path);
    m_model.TranslateCenterToOrigin();
}