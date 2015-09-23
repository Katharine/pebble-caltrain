#include <pebble.h>
//! @addtogroup InverterLayer
//! \brief Layer that inverts anything "below it".
//!
//! ![](inverter_layer.png)
//! This layer takes what has been drawn into the graphics context by layers
//! that are "behind" it in the layer hierarchy.
//! Then, the inverter layer uses its geometric information (bounds, frame) as
//! the area to invert in the graphics context. Inverting will cause black
//! pixels to become white and vice versa.
//!
//! The InverterLayer is useful, for example, to highlight the selected item
//! in a menu. In fact, the \ref MenuLayer itself uses InverterLayer to
//! accomplish its selection highlighting.
//! @{

struct InverterLayer;
typedef struct InverterLayer InverterLayer;

//! Creates a new InverterLayer on the heap and initializes it with the default values.
//! * Clips: `true`
//! * Hidden: `false`
//! @return A pointer to the InverterLayer. `NULL` if the InverterLayer could not
//! be created
InverterLayer* inverter_layer_create(GRect frame);

//! Destroys an InverterLayer previously created by inverter_layer_create
void inverter_layer_destroy(InverterLayer* inverter_layer);

//! Gets the "root" Layer of the inverter layer, which is the parent for the sub-
//! layers used for its implementation.
//! @param inverter_layer Pointer to the InverterLayer for which to get the "root" Layer
//! @return The "root" Layer of the inverter layer.
Layer* inverter_layer_get_layer(InverterLayer *inverter_layer);

//! Sets whether or not the window is fullscreen, consequently hiding the sytem status bar.
//! @note This needs to be called before pushing a window to the window stack.
//! @param window The window for which to set its full-screen property
//! @param enabled True to make the window full-screen or false to leave space for the system status bar.
//! @see \ref window_get_fullscreen()
void window_set_fullscreen(Window *window, bool enabled);

//! Gets whether the window is full-screen, consequently hiding the sytem status bar.
//! @param window The window for which to get its full-screen property
//! @return True if the window is marked as fullscreen, false if it is not marked as fullscreen.
bool window_get_fullscreen(const Window *window);
