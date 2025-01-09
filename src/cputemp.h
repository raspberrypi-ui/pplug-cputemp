#define MAX_NUM_SENSORS 10

typedef gint (*GetTempFunc) (char const *);

/* Private context for plugin */

typedef struct
{
    GtkWidget *plugin;                      /* Back pointer to the widget */
#ifdef LXPLUG
    LXPanel *panel;                         /* Back pointer to panel */
    config_setting_t *settings;
#else
    int icon_size;                          /* Variables used under wf-panel */
    gboolean bottom;
    GtkGesture *gesture;
#endif
    PluginGraph graph;
    guint timer;                            /* Timer for periodic update */
    int numsensors;
    char *sensor_array[MAX_NUM_SENSORS];
    GetTempFunc get_temperature[MAX_NUM_SENSORS];
    gint temperature[MAX_NUM_SENSORS];
    gboolean ispi;
    int lower_temp;                         /* Temperature of bottom of graph */
    int upper_temp;                         /* Temperature of top of graph */
    GdkRGBA foreground_colour;              /* Foreground colour for drawing area */
    GdkRGBA background_colour;              /* Background colour for drawing area */
    GdkRGBA low_throttle_colour;            /* Colour for bars with ARM freq cap */
    GdkRGBA high_throttle_colour;           /* Colour for bars with throttling */
} CPUTempPlugin;


