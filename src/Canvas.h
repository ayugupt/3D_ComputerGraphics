#pragma once

#include<gtkmm/drawingarea.h>

class Canvas: public Gtk::DrawingArea{
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
public:
    using Gtk::DrawingArea::DrawingArea;
};