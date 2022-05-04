#pragma once

#include<gtkmm/window.h>
#include<gtkmm/box.h>
#include<gtkmm/drawingarea.h>
#include "model/ObjModel.h"
#include<string>
#include "Canvas.h"
#include "Camera.h"
#include "MatrixFunctions.h"
#include<gtkmm/scale.h>
#include<gtkmm/adjustment.h>
#include<gtkmm/alignment.h>
#include<gtkmm/builder.h>
#include<giomm/simpleactiongroup.h>
#include<glibmm/refptr.h>
#include<gtkmm/overlay.h>
#include<gtkmm/entry.h>
#include<gtkmm/button.h>
#include<gtkmm/entrybuffer.h>
#include<gtkmm/frame.h>
#include<tinyexpr.h>
#include<memory>


class MainWindow: public Gtk::Window{

protected:

    Glib::RefPtr<Gio::SimpleActionGroup> m_simple_action_group;
    Glib::RefPtr<Gtk::Builder> m_builder;

    Gtk::Box m_body_box;

    Gtk::Box m_camera_position_box;

    Glib::RefPtr<Gtk::Adjustment> m_camera_position_x_adjustment;
    Glib::RefPtr<Gtk::Adjustment> m_camera_position_y_adjustment;
    Glib::RefPtr<Gtk::Adjustment> m_camera_position_z_adjustment;

    Gtk::Alignment m_camera_position_x_wrapper;
    Gtk::Alignment m_camera_position_y_wrapper;
    Gtk::Alignment m_camera_position_z_wrapper;

    Gtk::Scale m_camera_position_x;
    Gtk::Scale m_camera_position_y;
    Gtk::Scale m_camera_position_z;

    std::string m_menu_xml;

    Gtk::Overlay m_canvas_overlay;

    Gtk::Frame m_surface_function_frame;

    Gtk::Box m_surface_function_box;
    Gtk::Button m_confirm_function;

    Glib::RefPtr<Gtk::EntryBuffer> m_function_entry_buffer;
    Gtk::Entry m_function_entry;

    Canvas canvas;

    ObjModel m_model;
    Camera m_camera;

    te_expr *m_function_expression_ptr{nullptr};
    double m_x{}, m_y{};
    int m_expression_error;

    double m_pixels_per_unit{100};

    //-------------------Menu Actions-------------------------

    void OpenModel(); 

    //-------------------------------------------------------

    //------------------Render Functions--------------------

    void RenderOverlay();
    void RenderMenu();
    void RenderCameraControl();

    //-------------------------------------------------------

    void GetMenuXMLString(const char *file_location);
    void LoadModel(const char *file_path);
    
public:
    MainWindow();
    MainWindow(const char*fileName);

    ~MainWindow();

    //-----------------Signal Handlers---------------------------

    bool DrawHandler(const Cairo::RefPtr<Cairo::Context> &cr);
    void CameraPositionChanged();
    void OverlayFrameSizeAllocateHandler(Gtk::Allocation &allocation);

    void FunctionButtonHandler();
    
    
    //-----------------------------------------------------------

};