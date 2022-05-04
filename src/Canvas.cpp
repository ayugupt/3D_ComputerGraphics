#include "Canvas.h"
#include<iostream>

bool Canvas::on_draw(const Cairo::RefPtr<Cairo::Context> &cr){
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    cr->set_line_width(10.0);
    cr->set_source_rgb(1.0, 0.0, 0.0);

    // draw one line, every two pixels
    // without the 'fix', you won't notice any space between the lines,
    // since each one will occupy two pixels (width)
    cr->move_to(0, height/2);
    cr->line_to(width, height/2);
    cr->stroke();

    return true;
}