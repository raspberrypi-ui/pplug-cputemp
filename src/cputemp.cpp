/*============================================================================
Copyright (c) 2024 Raspberry Pi Holdings Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
============================================================================*/

#include <glibmm.h>
#include "gtk-utils.hpp"
#include "cputemp.hpp"

extern "C" {
    WayfireWidget *create () { return new WayfireCPUTemp; }
    void destroy (WayfireWidget *w) { delete w; }

    static constexpr conf_table_t conf_table[7] = {
        {CONF_COLOUR,   "foreground",   N_("Foreground colour")},
        {CONF_COLOUR,   "background",   N_("Background colour")},
        {CONF_COLOUR,   "throttle_1",   N_("Colour when ARM frequency capped")},
        {CONF_COLOUR,   "throttle_2",   N_("Colour when throttled")},
        {CONF_INT,      "low_temp",     N_("Lower temperature bound")},
        {CONF_INT,      "high_temp",    N_("Upper temperature bound")},
        {CONF_NONE,     NULL,           NULL}
    };
    const conf_table_t *config_params (void) { return conf_table; };
    const char *display_name (void) { return N_("CPU Temp"); };
    const char *package_name (void) { return GETTEXT_PACKAGE; };
}

void WayfireCPUTemp::bar_pos_changed_cb (void)
{
    if ((std::string) bar_pos == "bottom") cput->bottom = TRUE;
    else cput->bottom = FALSE;
}

void WayfireCPUTemp::icon_size_changed_cb (void)
{
    cput->icon_size = icon_size;
    cputemp_update_display (cput);
}

bool WayfireCPUTemp::set_icon (void)
{
    cputemp_update_display (cput);
    return false;
}

void WayfireCPUTemp::settings_changed_cb (void)
{
    if (!gdk_rgba_parse (&cput->foreground_colour, ((std::string) foreground_colour).c_str()))
        gdk_rgba_parse (&cput->foreground_colour, "dark gray");
    if (!gdk_rgba_parse (&cput->background_colour, ((std::string) background_colour).c_str()))
        gdk_rgba_parse (&cput->background_colour, "light gray");
    if (!gdk_rgba_parse (&cput->low_throttle_colour, ((std::string) throttle1_colour).c_str()))
        gdk_rgba_parse (&cput->low_throttle_colour, "orange");
    if (!gdk_rgba_parse (&cput->high_throttle_colour, ((std::string) throttle2_colour).c_str()))
        gdk_rgba_parse (&cput->high_throttle_colour, "red");

    cput->lower_temp = low_temp;
    cput->upper_temp = high_temp;

    cputemp_update_display (cput);
}

void WayfireCPUTemp::init (Gtk::HBox *container)
{
    /* Create the button */
    plugin = std::make_unique <Gtk::Button> ();
    plugin->set_name (PLUGIN_NAME);
    container->pack_start (*plugin, false, false);

    /* Setup structure */
    cput = g_new0 (CPUTempPlugin, 1);
    cput->plugin = (GtkWidget *)((*plugin).gobj());
    cput->icon_size = icon_size;
    icon_timer = Glib::signal_idle().connect (sigc::mem_fun (*this, &WayfireCPUTemp::set_icon));
    bar_pos_changed_cb ();

    /* Add long press for right click */
    gesture = add_longpress_default (*plugin);

    /* Initialise the plugin */
    cputemp_init (cput);

    /* Setup callbacks */
    icon_size.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::icon_size_changed_cb));
    bar_pos.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::bar_pos_changed_cb));

    foreground_colour.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::settings_changed_cb));
    background_colour.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::settings_changed_cb));
    throttle1_colour.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::settings_changed_cb));
    throttle2_colour.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::settings_changed_cb));
    low_temp.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::settings_changed_cb));
    high_temp.set_callback (sigc::mem_fun (*this, &WayfireCPUTemp::settings_changed_cb));

    settings_changed_cb ();
}

WayfireCPUTemp::~WayfireCPUTemp()
{
    icon_timer.disconnect ();
    cputemp_destructor (cput);
}

/* End of file */
/*----------------------------------------------------------------------------*/
