#include "simdisplay.h"
#include <cairomm/context.h>
#include <cmath>

// Constructor creates the world which contains all simulation information.
SimDisplay::SimDisplay()
{
    buttonpress = NULL;
    world = new World();
    
}

SimDisplay::~SimDisplay()
{
}

// When the mouse is clicked the simulation is progressed.
bool SimDisplay::on_button_press_event(GdkEventButton* event) {
  printf("on_button_press_event\n");
  buttonpress = event;
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &SimDisplay::perform_step), 500 );
  //GdkEventButton *ev;
  //ev->type = GDK_BUTTON_PRESS;
  //ev->window = event->window;

  //if(buttonpress == NULL){
    //perform_step();
    //buttonpress = event;
  //}
  //gdk_window_process_all_updates();
  //this->event((GdkEvent *)event);
  //gdk_window_process_all_updates();
}

bool SimDisplay::perform_step() {
  world->step();
  //gdk_window_invalidate_rect(get_window()->gobj(), NULL, true);
  get_window()->invalidate(true);
}

// When the window is first displayed or resized this event is called.
// We redraw the window
bool SimDisplay::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  bool expose = false;
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
    int lesser = MIN(width, height);

    // coordinates for the center of the window
    int xc, yc;
    xc = width / 2;
    yc = height / 2;

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->set_line_width(lesser * 0.00001);  // outline thickness changes
                                        // with window size

    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
    cr->clip();


    /* Draw all of our people */
    World::person *people = world->get_people();
    int i;
    for(i = 0; i < world->get_count(); i++) {
      cr->save();
      xc = people[i].x * width;
      yc = people[i].y * height;
      cr->arc(xc, yc, lesser / 200.0, 0.0, 2.0 * M_PI); // full circle
      if(people[i].status == 0)
        cr->set_source_rgba(0.0, 0.0, 1.0, 0.3);    // partially translucent
      else if(people[i].status < 0) 
        cr->set_source_rgba(0.0, 1.0, 0.0, 0.3);
      else 
        cr->set_source_rgba(((float)people[i].status)/DAYS_INFECTED, 0.0, 0.0, .3);
      cr->fill_preserve();
      cr->restore();  // back to opaque black
      cr->stroke();
    }
    //if(buttonpress != NULL) {
      //world->step();
      //this->event((GdkEvent *)buttonpress);
      //gdk_window_invalidate_rect(event->window, NULL, true);
      //this->event((GdkEvent *)buttonpress);
      //gdk_window_process_all_updates();
      //this->event((GdkEvent *)buttonpress);
    //}
  }
  //world->step();
  //window->invalidate(true);
  /*if(buttonpress != NULL) {
    GdkEventButton *ev;
    ev->type = GDK_BUTTON_PRESS;
    ev->window = event->window;

    this->event((GdkEvent *)ev);
  }*/
  //this->send_expose(new GtkEventExpose());
  return true;
}

