
#include <ext.h>
#include <ext_obex.h>
#include <jpatcher_api.h>
#include <jgraphics.h>
#include <z_dsp.h>

#include <AH_Memory_Swap.h>
#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>
#include <HIRT_Frame_Stats.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_spectrumdraw
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Defines

#define SPECTRUMDRAW_NUM_CURVES 4

#define FFTW_PI             3.14159265358979323846
#define FFTW_TWOPI          6.28318530717958647692
#define FFTW_THREEPI        9.42477796076937971538
#define FFTW_FOURPI         12.56637061435817295384
#define FFTW_SIXPI          18.84955592153875943076


// Curve style enum

enum t_curve_style
{
    DRAW_LINES = 0,
    DRAW_POINTS = 1,
    DRAW_POLY = 2,
    DRAW_LINE_POLY = 3,
};


// Subsample style enum

enum t_subsample_style
{
    SUBSAMPLE_FULL = 0,
    SUBSAMPLE_PEAK = 1,
    SUBSAMPLE_AVERAGE = 2,
};


// Window type enum

enum t_window_type {

    WIND_HANN,
    WIND_HAMMING,
    WIND_KAISER,
    WIND_TRIANGLE,
    WIND_BLACKMAN,
    WIND_BLACKMAN_62,
    WIND_BLACKMAN_70,
    WIND_BLACKMAN_74,
    WIND_BLACKMAN_92,
    WIND_BLACKMAN_HARRIS,
    WIND_FLAT_TOP,
    WIND_RECT
};


// Object class and structure

static t_class *this_class;

struct t_spectrumdraw
{
    t_pxjbox p_obj;                            // header for UI objects

    // Colours

    t_jrgba u_outline;
    t_jrgba u_background;
    t_jrgba u_hilite;
    t_jrgba u_marker;
    t_jrgba u_grid;
    t_jrgba u_tick;
    t_jrgba u_indicator;
    t_jrgba u_select;
    t_jrgba u_textcolor;
    t_jrgba u_textbox;
    t_jrgba u_displaytextcolor;
    t_jrgba curve_colors[SPECTRUMDRAW_NUM_CURVES];

    // Attributes and Parameters

    HIRT_COMMON_ATTR

    // Display Ranges

    double amp_range[2];
    double freq_range[2];

    // Markers

    double markers[64];

    long num_markers;

    // Flags

    long selection_on;
    long label_pos;
    long mouse_data_pos;
    long mouse_curve;
    long freq_display_options;

    long linear_mode;
    long mouse_mode;
    long phase_mode;

    long curve_style;
    long subsample_style;

    // Grid and Labels

    long grid_style;

    double freq_ref;
    double amp_ref;

    long freq_grid;
    long amp_grid;
    long phase_grid;

    long freq_labels;
    long amp_labels;
    long phase_labels;

    double oct_smooth;

    // Mousing Params

    AH_Boolean mouse_over;

    double mouse_x;
    double mouse_y;
    double mouse_sel_min_freq;
    double mouse_sel_max_freq;

    // Curve Data and Params

    void *sig_ins[SPECTRUMDRAW_NUM_CURVES];
    long sig_ins_valid[SPECTRUMDRAW_NUM_CURVES];

    t_safe_mem_swap curve_data[SPECTRUMDRAW_NUM_CURVES];

    t_safe_mem_swap realtime_io[SPECTRUMDRAW_NUM_CURVES];
    t_safe_mem_swap realtime_data[SPECTRUMDRAW_NUM_CURVES];
    t_safe_mem_swap realtime_stats[SPECTRUMDRAW_NUM_CURVES];

    long curve_mode[SPECTRUMDRAW_NUM_CURVES];
    long curve_chan[SPECTRUMDRAW_NUM_CURVES];
    long curve_freeze[SPECTRUMDRAW_NUM_CURVES];

    double curve_sr[SPECTRUMDRAW_NUM_CURVES];
    double curve_thickness[SPECTRUMDRAW_NUM_CURVES];

    // Realtime Params

    double redraw_time;
    double peak_hold;
    double time_smooth[2];
    double sample_rate;

    long fft_select;
    long zero_pad;
    long hop_pointer;
    long write_pointer;
    long window_type;

    t_safe_mem_swap realtime_temp;
    t_safe_mem_swap realtime_setup;

    t_int32_atomic draw_check;

    void *info_out;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Scale vals structure

struct t_scale_vals
{
    // Freq Params

    double min_freq;
    double max_freq;
    double width;

    // FFT Params

    AH_UIntPtr from;
    AH_UIntPtr to;

    double fft_ratio;
    double bin_min;
    double bin_scale;

    // Normal Scaling

    double x_min;
    double x_scale;

    double y_min;
    double y_max;
    double y_scale;

    // Labels and Grid Parameters

    double freq_ref;
    double freq_mul;

    double y_ref;
    double y_space;

    // Flags

    long log_mode;
    long pow_mode;
};


// Globals

double freq_grid_spacing[] = {0.0, 1.0/32.0, 1.0/24.0, 1.0/16.0, 1.0/12.0, 1.0/8.0, 1.0/4.0, 1.0/3.0, 1.0/2.0, 1.0, 1.5, 2.0, 3.0, 4.0};
double amp_grid_spacing[] = {0.0, 0.25, 0.5, 1.0, 2.5, 5.0, 10.0, 15.0, 20.0, 25.0, 50.0};
double phase_grid_spacing[] = {0.0, M_PI / 12.0, M_PI / 8.0, M_PI / 6.0, M_PI / 4.0, M_PI / 3.0, M_PI / 2.0, M_PI};
double freq_label_spacing[] = {0.0, 1.0, 2.0, 3.0, 4.0, 6.0, 8.0, 9.0, 12.0};
double amp_label_spacing[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,};
double phase_label_spacing[] = {0.0, 1.0, 2.0, 3.0, 4.0, 6.0, 8.0, 9.0, 12.0};
long phase_denominators[] = {0, 12, 8, 6, 4, 3, 2};
int amp_precisions[] = {0, 2, 1, 0, 1, 0, 0, 0, 0, 0, 0};

static const char *note_names[] = { "C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B" };

static double ftom_mul = 17.31234049066756088831909617202270564911975144983583;
static double ftom_add = -36.37631656229591524883618971458219761481675502447618;

t_jrgba no_transparency;


// Function prototypes

void *spectrumdraw_new(t_symbol *s, short argc, t_atom *argv);
void spectrumdraw_free(t_spectrumdraw *x);
void spectrumdraw_assist(t_spectrumdraw *x, void *b, long m, long a, char *s);

void free_fft_setup(void *setup);
void *alloc_fft_setup(AH_UIntPtr size, AH_UIntPtr nom_size);
void free_fft_stats(void *frame_stats);
void *alloc_realtime_data(AH_UIntPtr size, AH_UIntPtr nom_size);
void *alloc_fft_stats(AH_UIntPtr size, AH_UIntPtr nom_size);
void check_realtime_io(t_spectrumdraw *x, AH_UIntPtr fft_size);

void spectrumdraw_octave_smooth(double *in, float *out, AH_SIntPtr size, double oct_width);

void spectrumdraw_calc_selection_data(t_spectrumdraw *x);

void spectrumdraw_freeze(t_spectrumdraw *x, t_atom_long freeze_chan);
void spectrumdraw_select(t_spectrumdraw *x, t_symbol *sym, long argc, t_atom *argv);

double spectrumdraw_mouse_x_2_freq(t_spectrumdraw *x, t_object *patcherview, double x_pt);
void spectrumdraw_mouse_store_rel(t_spectrumdraw *x, t_object *patcherview, double x_pt, double y_pt);
void spectrumdraw_mousedown(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers);
void spectrumdraw_mousedrag(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers);
void spectrumdraw_mouseup(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers);
void spectrumdraw_mouseenter(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers);
void spectrumdraw_mouseleave(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers);
void spectrumdraw_mousemove(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers);

void spectrumdraw_buffer(t_spectrumdraw *x, t_symbol *sym, long argc, t_atom *argv);
void spectrumdraw_generate_window(t_spectrumdraw *x, AH_UIntPtr window_size, AH_UIntPtr fft_size);
void spectrumdraw_realtime(t_spectrumdraw *x, float *read_from, long phase_mode, long N);

t_int *spectrumdraw_perform(t_int *w);
void spectrumdraw_perform64(t_spectrumdraw *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void spectrumdraw_dsp_common(t_spectrumdraw *x, double samplerate, t_signal **sp, short *count);
void spectrumdraw_dsp(t_spectrumdraw *x, t_signal **sp, short *count);
void spectrumdraw_dsp64(t_spectrumdraw *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

long gcd(long a, long b);
void phase_label(char *text, long n, long d);
void amp_label(char *text, double amp, long amp_grid, int base_precision);
void frequency_label(char *text, double freq);

void spectrumdraw_jgraphics_paint_boxes(t_spectrumdraw *x, t_jgraphics *g, double width, double height, double sel_min_freq, double sel_max_freq, t_scale_vals *scale);
void spectrumdraw_jgraphics_paint_grid(t_spectrumdraw *x, t_jgraphics *g, double width, double height, t_scale_vals *scale);
void spectrumdraw_jgraphics_paint_ticks(t_spectrumdraw *x, t_jgraphics *g, double width, double height, t_scale_vals *scale);
void spectrumdraw_jgraphics_paint_markers(t_spectrumdraw *x, t_jgraphics *g, double width, double height, t_scale_vals *scale);
void spectrumdraw_jgraphics_curve_vertex(t_jgraphics *g, double x, double y, t_curve_style style);
void spectrumdraw_jgraphics_paint_curve_internal(t_spectrumdraw *x, t_jgraphics *g, AH_UIntPtr from, AH_UIntPtr to, float *y_vals, t_scale_vals *scale, double zoom_factor, double sub_sample_render, double width, double height, t_jrgba *color, t_curve_style curve_style, t_subsample_style sub_sample_style);
void spectrumdraw_jgraphics_paint_curve(t_spectrumdraw *x, t_jgraphics *g, AH_UIntPtr from, AH_UIntPtr to, float *y_vals, t_scale_vals *scale, double zoom_factor, double sub_sample_render, double width, double height, t_jrgba *color);
void spectrumdraw_jgraphics_paint(t_spectrumdraw *x, t_object *patcherview, t_scale_vals *scale, double zoom_factor, double sub_sample_render, t_rect rect, double sel_min_freq, double sel_max_freq);
void spectrumdraw_paint_labels(t_spectrumdraw *x, t_jgraphics *g, t_scale_vals *scale, double x_offset, double y_offset, double width, double height, double label_width, double label_height, double max_text_width, double max_text_height);
void spectrumdraw_get_measurements(t_spectrumdraw *x, double *return_width, double *return_height, t_rect *rect);
void spectrumdraw_paint_selection_data(t_spectrumdraw *x, t_jgraphics *g, float *y_vals, t_scale_vals *scale, AH_SIntPtr fft_size, double x_offset, double y_offset, double width, double height, double sel_min_freq, double sel_max_freq);
void spectrumdraw_paint(t_spectrumdraw *x, t_object *patcherview);

t_max_err spectrumdraw_notify(t_spectrumdraw *x, t_symbol *sym, t_symbol *msg, void *sender, void *data);


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Conversion Helpers //////////////////////////
//////////////////////////////////////////////////////////////////////////


t_jgraphics_text_justification combine_flags(t_jgraphics_text_justification a, t_jgraphics_text_justification b)
{
    int result = a | b;
    return (t_jgraphics_text_justification) result;
}

static inline double clip(double val, double min, double max)
{
    if (val < min)
        val = min;
    if (val > max)
        val = max;

    return val;
}


static inline double ftom(double a)
{
    return log(a) * ftom_mul + ftom_add;
}


static inline void ftonote(char *out, double a)
{
    double note = ftom(a) + 1200.0;
    double cents;
    long base_note = (long) round (note);

    cents = (note - base_note) * 100.0;

    if (note >= 0.0)
        sprintf(out, "%s%ld %+.1lf cents", note_names[base_note % 12], (base_note / 12) - 102, cents);
    else
        sprintf(out, "no note");
}


static inline double bin_2_freq(double bin, t_scale_vals *scale)
{
    return bin / scale->fft_ratio;
}


static inline double freq_2_bin(double freq, t_scale_vals *scale)
{
    return freq * scale->fft_ratio;
}


static inline double bin_2_mouse(double bin, t_scale_vals *scale)
{
    if (scale->log_mode)
        return scale->bin_scale * (log(bin) - scale->bin_min);
    else
        return scale->bin_scale * (bin - scale->bin_min);
}

/*
static inline double mouse_2_bin(double x_val, t_scale_vals *scale)
{
    if (scale->log_mode)
        return exp((x_val / scale->bin_scale) + scale->bin_min);
    else
        return (x_val / scale->bin_scale) + scale->bin_min;
}*/


static inline double freq_2_mouse(double freq, t_scale_vals *scale)
{
    if (scale->log_mode)
        return scale->x_scale * (log(freq) - scale->x_min);
    else
        return scale->x_scale * (freq - scale->x_min);
}


static inline double mouse_2_freq(double x_val, t_scale_vals *scale)
{
    if (scale->log_mode)
        return exp((x_val / scale->x_scale) + scale->x_min);
    else
        return (x_val / scale->x_scale) + scale->x_min;
}


static inline double conv_yval_2_mouse(double y_val, t_scale_vals *scale)
{
    // Assumes values are already converted from power to dB

    return scale->y_scale * (y_val - scale->y_max);
}

static inline double yval_2_mouse(double y_val, t_scale_vals *scale)
{
    // Will convert power value inputs

    if (scale->pow_mode)
        return scale->y_scale * (pow_to_db(y_val) - scale->y_max);
    else
        return scale->y_scale * (y_val - scale->y_max);
}

static inline double mouse_2_yval(double y_val, t_scale_vals *scale)
{
    return (y_val / scale->y_scale) + scale->y_max;
}


//////////////////////////////////////////////////////////////////////////
///////////////// Scaling Setup and Grid Precalculation //////////////////
//////////////////////////////////////////////////////////////////////////


static inline void spectrumdraw_grid_precalc(t_spectrumdraw *x, t_scale_vals *scale, long labels)
{
    double freq_ref = x->freq_ref;
    double end_ref;
    double y_space;
    double y_ref;
    double grid_mul = pow(2.0, labels ? freq_label_spacing[x->freq_labels] * freq_grid_spacing[x->freq_grid] : freq_grid_spacing[x->freq_grid]);

    if (x->freq_range[0] > x->freq_range[1])
    {
        grid_mul = 1.0 / grid_mul;
        end_ref = x->freq_range[0];
    }
    else
        end_ref = x->freq_range[0];

    freq_ref /= pow(grid_mul, ceil((log(freq_ref) - log(end_ref)) / log(grid_mul)));
    scale->freq_ref = freq_ref;
    scale->freq_mul = grid_mul;

    y_space = labels ? amp_label_spacing[x->amp_labels] * amp_grid_spacing[x->amp_grid] : amp_grid_spacing[x->amp_grid];
    y_ref = x->amp_ref;

    if (x->phase_mode)
    {
        y_ref = 0.0;
        y_space = labels ? phase_label_spacing[x->phase_labels] * phase_grid_spacing[x->phase_grid] : phase_grid_spacing[x->phase_grid];
    }

    scale->y_ref = conv_yval_2_mouse(y_ref, scale);
    scale->y_space = fabs(conv_yval_2_mouse(y_ref + y_space, scale) - scale->y_ref);
}


static inline void spectrumdraw_set_scale_vals(t_scale_vals *scale, t_rect *rect, double min_freq, double max_freq, double y_min, double y_max, long log_mode, long pow_mode)
{
    double x_min = min_freq;
    double x_max = max_freq;

    scale->log_mode = 0;

    if (log_mode)
    {
        x_min = log(x_min);
        x_max =    log(x_max);
        scale->log_mode = 1;
    }

    if (pow_mode)
        scale->pow_mode = 1;
    else
        scale->pow_mode = 0;

    scale->min_freq = min_freq;
    scale->max_freq = max_freq;

    scale->x_min = x_min;
    scale->y_min = y_min;
    scale->y_max = y_max;

    scale->x_scale = rect->width / (x_max - x_min);
    scale->y_scale = -rect->height / (y_max - y_min);

    scale->width = rect->width;
}


// Must be called after setting the basic scaling parameters

static inline void spectrumdraw_set_fft_scaling(t_scale_vals *scale, AH_UIntPtr fft_size, double sample_rate, double zoom)
{
    double x_min = scale->min_freq * fft_size / sample_rate;
    double x_max =  scale->max_freq * fft_size / sample_rate;

    scale->fft_ratio = fft_size / sample_rate;

    if (scale->log_mode)
    {
        x_min = log(x_min);
        x_max =    log(x_max);
    }

    scale->bin_min = x_min;
    scale->bin_scale = (scale->width / (x_max - x_min)) * zoom;
    scale->from = (AH_UIntPtr) floor(clip(scale->min_freq * scale->fft_ratio, 0, (double) (fft_size >> 1)));
    scale->to = (AH_UIntPtr) ceil(clip(scale->max_freq * scale->fft_ratio, 0, (double) (fft_size >> 1))) + 1;
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    t_class *c;
    short i;

    char chan_options[64];
    char temp_str[64];

    c = class_new("spectrumdraw~", (method)spectrumdraw_new, (method)spectrumdraw_free, sizeof(t_spectrumdraw), 0L, A_GIMME, 0);

    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    jbox_initclass(c, JBOX_FIXWIDTH | JBOX_COLOR | JBOX_FONTATTR);

    class_addmethod(c, (method)spectrumdraw_notify, "notify", A_CANT, 0);
    class_addmethod(c, (method)spectrumdraw_paint, "paint",    A_CANT, 0);

    class_addmethod(c, (method)spectrumdraw_mousedown, "mousedown",    A_CANT, 0);
    class_addmethod(c, (method)spectrumdraw_mousedrag, "mousedrag",    A_CANT, 0);
    class_addmethod(c, (method)spectrumdraw_mouseup, "mouseup",    A_CANT, 0);
    class_addmethod(c, (method)spectrumdraw_mouseenter, "mouseenter", A_CANT, 0);
    class_addmethod(c, (method)spectrumdraw_mouseleave,    "mouseleave", A_CANT, 0);
    class_addmethod(c, (method)spectrumdraw_mousemove, "mousemove", A_CANT, 0);

    class_addmethod(c, (method)spectrumdraw_select, "select", A_GIMME, 0);

    class_addmethod(c, (method)spectrumdraw_dsp, "dsp", A_CANT, 0L);
    class_addmethod(c, (method)spectrumdraw_dsp64, "dsp64", A_CANT, 0L);
    class_addmethod(c, (method)spectrumdraw_assist, "assist", A_CANT, 0);

    // Messages for state setting / retrieval

    class_addmethod(c, (method)spectrumdraw_freeze, "freeze", A_LONG, 0);
    class_addmethod(c, (method)spectrumdraw_buffer, "set", A_GIMME, 0);

    // Attributes

    /////////////////// Color Attributes ///////////////////

    CLASS_STICKY_ATTR(c, "category", 0, "Color");

    CLASS_ATTR_RGBA(c, "bgcolor", 0, t_spectrumdraw, u_background);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "bgcolor", 0, "1. 1. 1. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"bgcolor",0,"rgba","Background Color");

    CLASS_ATTR_RGBA(c, "bordercolor", 0, t_spectrumdraw, u_outline);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "bordercolor", 0, "0. 0. 0. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"bordercolor",0,"rgba","Border Color");

    CLASS_ATTR_RGBA(c, "indicatorcolor", 0, t_spectrumdraw, u_indicator);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "indicatorcolor", 0, "1. 0. 0. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"indicatorcolor",0,"rgba","Indicator Color");

    CLASS_ATTR_RGBA(c, "selectcolor", 0, t_spectrumdraw, u_select);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "selectcolor", 0, "0.4 0.4 0.4 0.4");
    CLASS_ATTR_STYLE_LABEL(c,"selectcolor",0,"rgba","Selection Color");

    CLASS_ATTR_RGBA(c, "markercolor", 0, t_spectrumdraw, u_marker);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "markercolor", 0, "1. 0. 0. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"markercolor",0,"rgba","Marker Color");

    CLASS_ATTR_RGBA(c, "tickcolor", 0, t_spectrumdraw, u_tick);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "tickcolor", 0, "1. 1. 1. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"tickcolor",0,"rgba","Tick Color");

    CLASS_ATTR_RGBA(c, "gridcolor", 0, t_spectrumdraw, u_grid);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "gridcolor", 0, "0.4 0.4 0.4 1.");
    CLASS_ATTR_STYLE_LABEL(c,"gridcolor",0,"rgba","Grid Color");

    CLASS_ATTR_RGBA(c, "textboxcolor", 0, t_spectrumdraw, u_textbox);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "textboxcolor", 0, "1. 1. 1. 0.9");
    CLASS_ATTR_STYLE_LABEL(c,"textboxcolor",0,"rgba","Text Box Color");

    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "color", 0, "0.5 0. 0. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"color",0,"rgba","Curve Color");

    for (i = 1; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        char attrib_name[32];
        char display_name[32];
        sprintf(attrib_name, "color%d", i + 1);
        sprintf(display_name, "Curve Color %d", i + 1);
        sprintf(temp_str, "%lf %lf %lf 1.", (i == 3) ? 0.5 : 0.0, (i == 1) || (i == 3) ? 0.5 : 0.0, i == 2 ? 0.5 : 0.0);

        CLASS_ATTR_RGBA(c, attrib_name, 0, t_spectrumdraw, curve_colors[i]);
        CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, attrib_name, 0, temp_str);
        CLASS_ATTR_STYLE_LABEL(c, attrib_name,0,"rgba", display_name);
    }

    // Do text color

    CLASS_ATTR_RGBA(c, "textcolor", 0, t_spectrumdraw, u_textcolor);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "textcolor", 0, "0. 0. 0. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"textcolor",0,"rgba","Label Text Color");

    CLASS_ATTR_RGBA(c, "displaytextcolor", 0, t_spectrumdraw, u_displaytextcolor);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "displaytextcolor", 0, "0. 0. 0. 1.");
    CLASS_ATTR_STYLE_LABEL(c,"displaytextcolor",0,"rgba","Display Text Color");

    CLASS_STICKY_ATTR_CLEAR(c, "category");

    /////////////////// Curve Attributes ///////////////////

    CLASS_STICKY_ATTR(c, "category", 0, "Curves");

    // Mode Attributes

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        char attrib_name[32];
        char display_name[32];

        if (!i)
        {
            sprintf(attrib_name, "mode");
            sprintf(display_name, "Mode");
        }
        else
        {
            sprintf(attrib_name, "mode%d", i + 1);
            sprintf(display_name, "Mode %d", i + 1);
        }

        CLASS_ATTR_LONG(c, attrib_name, 0, t_spectrumdraw, curve_mode[i]);
        CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, attrib_name, 0, "1");
        CLASS_ATTR_ENUMINDEX(c, attrib_name, 0, "Off Normal Peak Smooth Accumulate");
        CLASS_ATTR_FILTER_CLIP(c, attrib_name, 0, 4);
        CLASS_ATTR_LABEL(c,attrib_name,0,display_name);
    }

    strcpy(chan_options, "1");

    // Chan Attributes

    for (i = 1; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        strcpy(temp_str, chan_options);
        sprintf(chan_options, "%s %d", temp_str, i + 1);
    }

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        char attrib_name[32];
        char display_name[32];

        if (!i)
        {
            sprintf(attrib_name, "chan");
            sprintf(display_name, "Input Channel");
        }
        else
        {
            sprintf(attrib_name, "chan%d", i + 1);
            sprintf(display_name, "Input Channel %d", i + 1);
        }

        sprintf(temp_str, "%d", i + 1);

        CLASS_ATTR_LONG(c, attrib_name, 0, t_spectrumdraw, curve_chan[i]);
        CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, attrib_name, 0, temp_str);
        CLASS_ATTR_FILTER_CLIP(c, attrib_name, 1, SPECTRUMDRAW_NUM_CURVES);
        CLASS_ATTR_LABEL(c,attrib_name,0,display_name);
    }

    // Thickness Attributes

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        char attrib_name[32];
        char display_name[32];

        if (!i)
        {
            sprintf(attrib_name, "thickness");
            sprintf(display_name, "Curve Thickness");
        }
        else
        {
            sprintf(attrib_name, "thickness%d", i + 1);
            sprintf(display_name, "Curve Thickness %d", i + 1);
        }

        CLASS_ATTR_DOUBLE(c, attrib_name, 0, t_spectrumdraw, curve_thickness[i]);
        CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, attrib_name, 0, "1.");
        CLASS_ATTR_FILTER_CLIP(c, attrib_name, 1.0, 4.0);
        CLASS_ATTR_LABEL(c,attrib_name, 0, display_name);
    }

    CLASS_STICKY_ATTR_CLEAR(c, "category");

    /////////////////// Display Attributes ///////////////////

    CLASS_STICKY_ATTR(c, "category", 0, "Display");

    declare_HIRT_common_attributes(c);

    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "readchan", 0, "1");

    CLASS_ATTR_LONG(c, "phasemode", 0, t_spectrumdraw, phase_mode);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "phasemode", 0, "0");
    CLASS_ATTR_STYLE_LABEL(c,"phasemode",0,"onoff","Phase Mode");

    CLASS_ATTR_LONG(c, "linearmode", 0, t_spectrumdraw, linear_mode);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "linearmode", 0, "0");
    CLASS_ATTR_STYLE_LABEL(c,"linearmode",0,"onoff","Linear Mode");

    CLASS_ATTR_LONG(c, "mousemode", 0, t_spectrumdraw, mouse_mode);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "mousemode", 0, "1");
    CLASS_ATTR_ENUMINDEX(c,"mousemode", 0, "None Mouse Curve \"Mouse + Selection\" \"Curve + Selection\"");
    CLASS_ATTR_FILTER_CLIP(c, "mousemode", 0, 4);
    CLASS_ATTR_LABEL(c,"mousemode",0,"Mousing Mode");

    CLASS_ATTR_LONG(c, "mousedatapos", 0, t_spectrumdraw, mouse_data_pos);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "mousedatapos", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"mousedatapos", 0, "\"Top Left\" \"Top Right\" \"Bottom Left\" \"Bottom Right\"");
    CLASS_ATTR_FILTER_CLIP(c, "mousedatapos", 0, 3);
    CLASS_ATTR_LABEL(c,"mousedatapos",0,"Mouse Data Position");

    CLASS_ATTR_LONG(c, "mousecurve", 0, t_spectrumdraw, mouse_curve);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "mousecurve", 0, "1");
    CLASS_ATTR_FILTER_CLIP(c, "mousecurve", 1, SPECTRUMDRAW_NUM_CURVES);
    CLASS_ATTR_LABEL(c,"mousecurve",0,"Mouse Curve");

    CLASS_ATTR_LONG(c, "freqstyle", 0, t_spectrumdraw, freq_display_options);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "freqstyle", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"freqstyle", 0, "Freq Note \"Freq + Note\"");
    CLASS_ATTR_FILTER_CLIP(c, "freqstyle", 0, 2);
    CLASS_ATTR_LABEL(c,"freqstyle",0,"Frequency Display Style");

    CLASS_ATTR_LONG(c, "curvestyle", 0, t_spectrumdraw, curve_style);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "curvestyle", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"curvestyle", 0, "Lines Dots Fill \"Line + Fill\"");
    CLASS_ATTR_FILTER_CLIP(c, "curvestyle", 0, 3);
    CLASS_ATTR_LABEL(c,"curvestyle",0,"Curve Style");

    CLASS_ATTR_DOUBLE(c, "octavesmooth", 0, t_spectrumdraw, oct_smooth);
    CLASS_ATTR_FILTER_CLIP(c, "octavesmooth", 0.0, 1.0);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "octavesmooth", 0, "0.");
    CLASS_ATTR_LABEL(c,"octavesmooth",0,"Octave Smoothing");

    CLASS_ATTR_LONG(c, "subsamplestyle", 0, t_spectrumdraw, subsample_style);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "subsamplestyle", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"subsamplestyle", 0, "Full Peak Average");
    CLASS_ATTR_FILTER_CLIP(c, "subsamplestyle", 0, 2);
    CLASS_ATTR_LABEL(c,"subsamplestyle",0,"Subsample Style");

    CLASS_ATTR_DOUBLE_ARRAY(c, "freqrange", 0, t_spectrumdraw, freq_range, 2);
    CLASS_ATTR_FILTER_MIN(c, "freqrange", 0.0001);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "freqrange", 0, "20. 22050.");
    CLASS_ATTR_LABEL(c, "freqrange", 0 , "Frequency Range (Hz)");

    CLASS_ATTR_DOUBLE_ARRAY(c, "amprange", 0, t_spectrumdraw, amp_range, 2);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "amprange", 0, "-80 20");
    CLASS_ATTR_LABEL(c, "amprange", 0 , "Amplitude Range (dB)");

    CLASS_ATTR_DOUBLE_VARSIZE(c, "markers", 0, t_spectrumdraw, markers, num_markers, 32);
    CLASS_ATTR_FILTER_MIN(c, "markers", 1.0);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "markers", 0, "");
    CLASS_ATTR_LABEL(c, "markers", 0 , "Markers (Hz)");

    CLASS_STICKY_ATTR_CLEAR(c, "category");

    /////////////////// Grid Attributes ///////////////////

    CLASS_STICKY_ATTR(c, "category", 0, "Grid/Labels");

    CLASS_ATTR_LONG(c, "gridstyle", 0, t_spectrumdraw, grid_style);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "gridstyle", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"gridstyle", 0, "Normal Plaid Ticks \"Plaid + Ticks\"");
    CLASS_ATTR_FILTER_CLIP(c, "gridstyle", 0, 3);
    CLASS_ATTR_LABEL(c,"gridstyle",0,"Grid Style");

    CLASS_ATTR_LONG(c, "freqgrid", 0, t_spectrumdraw, freq_grid);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "freqgrid", 0, "7");
    CLASS_ATTR_ENUMINDEX(c,"freqgrid", 0, "Off 1/32 1/24 1/16 1/12 1/8 1/4 1/3 1/2 1 1.5 2 3 4");
    CLASS_ATTR_FILTER_CLIP(c, "freqgrid", 0, 13);
    CLASS_ATTR_LABEL(c,"freqgrid",0,"Freq Grid (octaves)");

    CLASS_ATTR_LONG(c, "ampgrid", 0, t_spectrumdraw, amp_grid);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "ampgrid", 0, "6");
    CLASS_ATTR_ENUMINDEX(c,"ampgrid", 0, "Off 0.25 0.5 1 2.5 5 10 15 20 25 50");
    CLASS_ATTR_FILTER_CLIP(c, "ampgrid", 0, 10);
    CLASS_ATTR_LABEL(c,"ampgrid",0,"Amp Grid (dB)");

    CLASS_ATTR_LONG(c, "phasegrid", 0, t_spectrumdraw, phase_grid);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "phasegrid", 0, "4");
    CLASS_ATTR_ENUMINDEX(c,"phasegrid", 0, "Off π/12 π/8 π/6 π/4 π/3 π/2 π");
    CLASS_ATTR_FILTER_CLIP(c, "phasegrid", 0, 7);
    CLASS_ATTR_LABEL(c,"phasegrid",0,"Phase Grid");

    CLASS_ATTR_DOUBLE(c, "freqref", 0, t_spectrumdraw, freq_ref);
    CLASS_ATTR_FILTER_MIN(c, "freqref", 1.0);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "freqref", 0, "1000");
    CLASS_ATTR_LABEL(c,"freqref",0,"Freq Reference (Hz)");

    CLASS_ATTR_DOUBLE(c, "ampref", 0, t_spectrumdraw, amp_ref);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "ampref", 0, "0");
    CLASS_ATTR_LABEL(c,"ampref",0,"Amp Reference (dB)");

    CLASS_ATTR_LONG(c, "freqlabels", 0, t_spectrumdraw, freq_labels);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "freqlabels", 0, "3");
    CLASS_ATTR_ENUMINDEX(c,"freqlabels", 0, "Off 1 2 3 4 6 8 9 12");
    CLASS_ATTR_FILTER_CLIP(c, "freqlabels", 0, 8);
    CLASS_ATTR_LABEL(c,"freqlabels",0,"Freq Labels (grid points)");

    CLASS_ATTR_LONG(c, "amplabels", 0, t_spectrumdraw, amp_labels);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "amplabels", 0, "1");
    CLASS_ATTR_ENUMINDEX(c,"amplabels", 0, "Off 1 2 3 4 5 6 7 8 9 10");
    CLASS_ATTR_FILTER_CLIP(c, "amplabels", 0, 10);
    CLASS_ATTR_LABEL(c,"amplabels",0,"Amp Labels (grid points)");

    CLASS_ATTR_LONG(c, "phaselabels", 0, t_spectrumdraw, phase_labels);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "phaselabels", 0, "1");
    CLASS_ATTR_ENUMINDEX(c,"phaselabels", 0, "Off 1 2 3 4 5 6 7 8 9 10");
    CLASS_ATTR_FILTER_CLIP(c, "phaselabels", 0, 10);
    CLASS_ATTR_LABEL(c,"phaselabels",0,"Phase Labels (grid points)");

    CLASS_ATTR_LONG(c, "labelpos", 0, t_spectrumdraw, label_pos);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "labelpos", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"labelpos", 0, "\"Top Left\" \"Top Right\" \"Bottom Left\" \"Bottom Right\"");
    CLASS_ATTR_FILTER_CLIP(c, "labelpos", 0, 3);
    CLASS_ATTR_LABEL(c,"labelpos",0,"Label Positions");

    CLASS_STICKY_ATTR_CLEAR(c, "category");

    /////////////////// Realtime Attributes ///////////////////

    CLASS_STICKY_ATTR(c, "category", 0, "Realtime");

    CLASS_ATTR_LONG(c, "fftsize", 0, t_spectrumdraw, fft_select);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "fftsize", 0, "4");
    CLASS_ATTR_ENUMINDEX(c,"fftsize", 0, "256 512 1024 2048 4096 8192 16384 32768 65536");
    CLASS_ATTR_FILTER_CLIP(c, "fftsize", 0, 8);
    CLASS_ATTR_LABEL(c,"fftsize",0,"FFT Size");

    CLASS_ATTR_DOUBLE(c, "interval", 0, t_spectrumdraw, redraw_time);
    CLASS_ATTR_FILTER_MIN(c, "interval", 10);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "interval", 0, "50");
    CLASS_ATTR_LABEL(c,"interval",0,"Polling Interval (ms)");

    CLASS_ATTR_DOUBLE(c, "peakhold", 0, t_spectrumdraw, peak_hold);
    CLASS_ATTR_FILTER_MIN(c, "peakhold", 10);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "peakhold", 0, "3000");
    CLASS_ATTR_LABEL(c,"peakhold",0,"Peak Hold Time (ms)");

    CLASS_ATTR_DOUBLE_ARRAY(c, "timesmooth", 0, t_spectrumdraw, time_smooth, 2);
    CLASS_ATTR_FILTER_MIN(c, "timesmooth", 1.0);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "timesmooth", 0, "10. 3000.");
    CLASS_ATTR_LABEL(c, "timesmooth", 0 , "Time Smoothing Up / Down (ms)");

    CLASS_ATTR_LONG(c, "windowtype", 0, t_spectrumdraw, window_type);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "windowtype", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"windowtype", 0, "\"Hann\" Hamming Kaiser Triangle Blackman \"Blackman 62\" \"Blackman 70\" \"Blackman 74\" \"Blackman 92\" Blackman-Harris \"Flat Top\" Rectangle");
    CLASS_ATTR_FILTER_CLIP(c, "windowtype", 0, 11);
    CLASS_ATTR_LABEL(c,"windowtype",0,"Window Type");

    CLASS_ATTR_LONG(c, "zeropad", 0, t_spectrumdraw, zero_pad);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "zeropad", 0, "0");
    CLASS_ATTR_ENUMINDEX(c,"zeropad", 0, "Off 2x 4x 8x 16x");
    CLASS_ATTR_FILTER_CLIP(c, "zeropad", 0, 4);
    CLASS_ATTR_LABEL(c,"zeropad",0,"Zero Padding");

    CLASS_STICKY_ATTR_CLEAR(c, "category");

    CLASS_ATTR_DEFAULT(c,"patching_rect",0, "0. 0. 400 160.");

    class_dspinitjbox(c);
    class_register(CLASS_BOX, c);
    this_class = c;

    no_transparency.red = 0.0;
    no_transparency.green = 0.0;
    no_transparency.blue = 0.0;
    no_transparency.alpha = 1.0;

    return 0;
}


void *spectrumdraw_new(t_symbol *s, short argc, t_atom *argv)
{
    t_spectrumdraw *x = NULL;
     t_dictionary *d = NULL;
    long boxflags;
    short i;

    if (!(d = object_dictionaryarg(argc,argv)))
        return NULL;

    x = (t_spectrumdraw *)object_alloc(this_class);
    boxflags = 0 | JBOX_DRAWFIRSTIN | JBOX_NODRAWBOX | JBOX_DRAWINLAST | JBOX_TRANSPARENT | JBOX_GROWBOTH;

    jbox_new((t_jbox *)x, boxflags, argc, argv);
    x->p_obj.z_box.b_firstin = (t_object *) x;

    dsp_setupjbox((t_pxjbox *)x, SPECTRUMDRAW_NUM_CURVES);

    init_HIRT_common_attributes(x);

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        x->sig_ins[i] = 0;
        x->sig_ins_valid[i] = 0;
    }

    // Init once per object memory

    alloc_mem_swap(&x->realtime_temp, 0, 0);
    alloc_mem_swap(&x->realtime_setup, 0, 0);

    // Init per curve / channel memory

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        alloc_mem_swap(x->realtime_data + i, 0, 0);
        alloc_mem_swap(x->realtime_stats + i, 0, 0);
        alloc_mem_swap(x->curve_data + i, 0, 0);
        alloc_mem_swap(x->curve_data + i, 0, 0);
        x->curve_freeze[i] = 0;
    }

    x->mouse_over = false;
    x->selection_on = 0;

    x->write_pointer = 0;
    x->hop_pointer = 0;

    x->draw_check = 0;

    x->sample_rate = sys_getsr();

    x->info_out = outlet_new(x, 0);

    attr_dictionary_process(x,d);
    jbox_ready((t_jbox *)x);

    return x;
}


void spectrumdraw_free(t_spectrumdraw *x)
{
    short i;

    dsp_freejbox((t_pxjbox *)x);
    jbox_free((t_jbox *)x);

    // Once per object

    free_mem_swap(&x->realtime_temp);
    free_mem_swap(&x->realtime_setup);

    // Once per curve / channel

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        free_mem_swap(x->curve_data + i);
        free_mem_swap(x->realtime_io + i);
        free_mem_swap(x->realtime_data + i);
        free_mem_swap(x->realtime_stats + i);
    }

    free_HIRT_common_attributes(x);
}


void spectrumdraw_assist(t_spectrumdraw *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
        sprintf(s, "Mousing Info");
    else
    {
        if (a == 0)
            sprintf(s,"(signal) Audio Input 1 / Instructions In");
        else
            sprintf(s,"(signal) Audio Input %ld", a + 1);
    }
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Realtime Memory Allocation ///////////////////////
//////////////////////////////////////////////////////////////////////////


void free_fft_setup(void *setup)
{
    hisstools_destroy_setup((FFT_SETUP_D) setup);
}


void *alloc_fft_setup(AH_UIntPtr size, AH_UIntPtr nom_size)
{
    FFT_SETUP_D setup;
    hisstools_create_setup(&setup, size);
    return setup;
}


void free_fft_stats(void *frame_stats)
{
    destroy_frame_stats((t_frame_stats *)frame_stats);
}


void *alloc_realtime_data(AH_UIntPtr size, AH_UIntPtr nom_size)
{
    void *ptr = allocate_aligned<uint8>(size);

    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}


void *alloc_fft_stats(AH_UIntPtr size, AH_UIntPtr nom_size)
{
    return create_frame_stats(size);
}


void check_realtime_io(t_spectrumdraw *x, AH_UIntPtr fft_size)
{
    short i;

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        if (x->sig_ins_valid[i])
            schedule_equal_mem_swap_custom(&x->realtime_io[i], alloc_realtime_data, 0, fft_size * 2 * sizeof(float), fft_size);
        else
            clear_mem_swap(&x->realtime_io[i]);

        if (x->sig_ins_valid[x->curve_chan[i] - 1] && x->curve_mode[i] && !x->curve_freeze[i])
        {
            void *ptr = schedule_equal_mem_swap_custom(&x->realtime_data[i], alloc_realtime_data, 0, (fft_size + 2) * sizeof(float), fft_size);
            schedule_equal_mem_swap_custom(&x->realtime_stats[i], alloc_fft_stats, free_fft_stats, fft_size + 1, fft_size);
            schedule_swap_mem_swap(&x->curve_data[i], ptr, fft_size);
            x->curve_sr[i] = x->sample_rate;
        }
        else
        {
            if (!x->sig_ins_valid[x->curve_chan[i] - 1] || !x->curve_mode[i])
            {
                clear_mem_swap(&x->realtime_data[i]);
                clear_mem_swap(&x->realtime_stats[i]);

                if (x->curve_freeze[i] != 2)
                    clear_mem_swap(&x->curve_data[i]);
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Octave Smoothing ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_octave_smooth(double *in, float *out, AH_SIntPtr size, double oct_width)
{
    AH_SIntPtr lo;
    AH_SIntPtr hi;
    AH_SIntPtr i;

    if (oct_width)
    {
        oct_width /= 2.0;
        oct_width = pow(2.0, oct_width);

        for (i = 1; i < size; i++)
            in[i] += in[i - 1];

        out[0] = (float) in[0];

        for (i = 1; i < size; i++)
        {
            lo = (AH_SIntPtr) (i / oct_width);
            hi = (AH_SIntPtr) (i * oct_width);

            if (lo == hi)
                lo--;

            if (hi > size - 1)
                hi = size - 1;

            out[i] = (float) ((in[hi] - in[lo]) / (hi - lo));
        }
    }
    else
    {

        for (i = 0; i < size; i++)
            out[i] = (float) in[i];
    }
}


//////////////////////////////////////////////////////////////////////////
////////////////////////// Selection Calculation /////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_calc_selection_data(t_spectrumdraw *x)
{
    // Selection + Point Stuff

    double sel_min_freq = x->mouse_sel_min_freq;
    double sel_max_freq = x->mouse_sel_max_freq;
    double sel_avg_amp = 0;
    double sel_min_amp = HUGE_VAL;
    double sel_max_amp = -HUGE_VAL;
    double sel_min_amp_freq;
    double sel_max_amp_freq;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double mouse_x = x->mouse_x;
    double mouse_y = x->mouse_y;
    double x_val;
    double y_val;
    double fft_ratio;
    double bin_pos;

    AH_SIntPtr read_lo;
    AH_SIntPtr read_hi;
    AH_SIntPtr sel_from;
    AH_SIntPtr sel_to;
    AH_SIntPtr sel_min = 0;
    AH_SIntPtr sel_max = 0;
    AH_SIntPtr fft_size;
    AH_SIntPtr i;

    long log_mode = !x->linear_mode;

    t_atom argv[10];

    AH_Boolean inbox = mouse_x >= 0 && mouse_x <= 1 && mouse_y >= 0 && mouse_y <= 1;

    float *y_vals;

    // Try to swap memory

    attempt_mem_swap(&x->curve_data[x->mouse_curve - 1]);

    // Get Data

    y_vals = (float *) x->curve_data[x->mouse_curve - 1].current_ptr;
    fft_size = x->curve_data[x->mouse_curve - 1].current_size;
    fft_ratio = fft_size / x->curve_sr[x->mouse_curve - 1];
    sel_from = (AH_SIntPtr) ceil(sel_min_freq * fft_ratio);
    sel_to = (AH_SIntPtr) floor(sel_max_freq * fft_ratio);

    // Check phase mode

    if (x->phase_mode)
    {
        y_vals += (x->curve_data[x->mouse_curve - 1].current_size >> 1) + 1;
        y_min = -M_PI;
        y_max = M_PI;
    }
    else
    {
        y_min = x->amp_range[0];
        y_max = x->amp_range[1];
    }

    // Check frequency mode

    x_min = x->freq_range[0];
    x_max = x->freq_range[1];

    if (log_mode)
    {
        x_min = log(x_min);
        x_max =    log(x_max);
    }

    // Sanity Check

    sel_from = (AH_SIntPtr) clip((double) sel_from, 0, (double) (fft_size / 2 - 1));
    sel_to = (AH_SIntPtr) clip((double) sel_to, 0, (double) (fft_size / 2));

    // Clip mouse positions and convert

    mouse_x = clip(mouse_x, 0, 1);
    mouse_y = clip(mouse_y, 0, 1);

    if ((x->mouse_over == true && x->mouse_mode && inbox) || x->selection_on)
    {
        if (!x->selection_on)
        {
            if (log_mode)
                x_val = exp((mouse_x * (x_max - x_min)) + x_min);
            else
                x_val = (mouse_x * (x_max - x_min)) + x_min;

            // Should ideally do cubic interpolation of dB vals here!

            if ((x->mouse_mode == 2 || x->mouse_mode == 4))
            {
                bin_pos = x_val * fft_ratio;
                read_lo = (AH_SIntPtr) floor(bin_pos);
                read_hi = read_lo + 1;

                if (!fft_size)
                    return;

                // Safefy Check

                if (read_lo > (fft_size >> 1))
                    read_lo = fft_size >> 1;

                if (read_hi > (fft_size >> 1))
                    read_hi = fft_size >> 1;

                y_val = y_vals[read_lo] + (bin_pos - read_lo) * (y_vals[read_hi] - y_vals[read_lo]);

                if (!x->phase_mode)
                    y_val = pow_to_db(y_val);
            }
            else
                y_val = (mouse_y * (y_min - y_max)) + y_max;

            atom_setfloat(argv+0, x_val);
            atom_setfloat(argv+1, y_val);

            outlet_anything(x->info_out, gensym("mouse"), 2, argv);
        }
        else
        {
            if (!fft_size)
                return;

            for (i = sel_from; i < sel_to; i++)
            {
                double test_val = y_vals[i];

                sel_avg_amp += test_val;

                if (test_val > sel_max_amp)
                {
                    sel_max_amp = test_val;
                    sel_max = i;
                }
                if (test_val < sel_min_amp)
                {
                    sel_min_amp = test_val;
                    sel_min = i;
                }
            }

            sel_avg_amp /= (double) (sel_to - sel_from);
            sel_min_amp_freq = sel_min / fft_ratio;
            sel_max_amp_freq = sel_max / fft_ratio;

            atom_setfloat(argv+0, sel_min_freq);
            atom_setfloat(argv+1, sel_max_freq);
            atom_setfloat(argv+2, sel_min_amp_freq);
            atom_setfloat(argv+3, sel_max_amp_freq);
            atom_setfloat(argv+4, pow_to_db(sel_min_amp));
            atom_setfloat(argv+5, pow_to_db(sel_max_amp));
            atom_setfloat(argv+6, pow_to_db(sel_avg_amp));

            outlet_anything(x->info_out, gensym("select"), 7, argv);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// User Messages /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_freeze(t_spectrumdraw *x, t_atom_long freeze_chan)
{
    freeze_chan = (freeze_chan > SPECTRUMDRAW_NUM_CURVES) ? SPECTRUMDRAW_NUM_CURVES : freeze_chan;
    freeze_chan = (--freeze_chan < 0) ? 0 : freeze_chan;

    x->curve_freeze[freeze_chan] = 1;

    check_realtime_io (x, (AH_UIntPtr) 1 << (x->fft_select + 8));
}


void spectrumdraw_select(t_spectrumdraw *x, t_symbol *sym, long argc, t_atom *argv)
{
    if (argc < 2)
        x->selection_on = 0;
    else
    {
        x->selection_on = 1;
        x->mouse_sel_min_freq = atom_getfloat(argv++);
        x->mouse_sel_max_freq = atom_getfloat(argv++);
    }

    spectrumdraw_calc_selection_data(x);

    jbox_invalidate_layer((t_object*)x, NULL, gensym("background_layer"));
    jbox_redraw((t_jbox *)x);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Mouse Interaction ///////////////////////////
//////////////////////////////////////////////////////////////////////////


double spectrumdraw_mouse_x_2_freq(t_spectrumdraw *x, t_object *patcherview, double x_pt)
{
    double text_width, text_height;
    t_scale_vals scale;
    t_rect rect;

    jbox_get_rect_for_view((t_object *) x, patcherview, &rect);

    spectrumdraw_get_measurements(x, &text_width, &text_height, &rect);

    // Set all scale values, although we only need a subset of them

    spectrumdraw_set_scale_vals(&scale, &rect, x->freq_range[0], x->freq_range[1], 0, 1, !x->linear_mode, !x->phase_mode);
    return mouse_2_freq(clip(x_pt - rect.x, 0, rect.width), &scale);
}


void spectrumdraw_mouse_store_rel(t_spectrumdraw *x, t_object *patcherview, double x_pt, double y_pt)
{
    double text_width, text_height;
    t_rect rect;

    jbox_get_rect_for_view((t_object *) x, patcherview, &rect);
    spectrumdraw_get_measurements(x, &text_width, &text_height, &rect);

    x->mouse_x = (x_pt - rect.x) / rect.width;
    x->mouse_y = (y_pt - rect.y) / rect.height;
}


void spectrumdraw_mousedown(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if (x->mouse_mode < 3)
        return;

    x->selection_on = 0;
    x->mouse_sel_min_freq = spectrumdraw_mouse_x_2_freq(x, patcherview, pt.x);

    jbox_invalidate_layer((t_object*)x, NULL, gensym("background_layer"));
    jbox_redraw((t_jbox *)x);
}


void spectrumdraw_mousedrag(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers)
{
    if (x->mouse_mode < 3)
        return;

    x->selection_on = 1;
    x->mouse_sel_max_freq = spectrumdraw_mouse_x_2_freq(x, patcherview, pt.x);

    spectrumdraw_calc_selection_data(x);

    jbox_invalidate_layer((t_object*)x, NULL, gensym("background_layer"));
    jbox_redraw((t_jbox *)x);
}


void spectrumdraw_mouseup(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers)
{
}


void spectrumdraw_mouseenter(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->mouse_over = true;
    spectrumdraw_mouse_store_rel(x,  patcherview, pt.x, pt.y);

    spectrumdraw_calc_selection_data(x);

    jbox_redraw((t_jbox *)x);
}


void spectrumdraw_mouseleave(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->mouse_over = false;
    jbox_redraw((t_jbox *)x);
}


void spectrumdraw_mousemove(t_spectrumdraw *x, t_object *patcherview, t_pt pt, long modifiers)
{
    spectrumdraw_mouse_store_rel(x,  patcherview, pt.x, pt.y);
    spectrumdraw_calc_selection_data(x);
    jbox_redraw((t_jbox *)x);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Buffer Analysis ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_buffer(t_spectrumdraw *x, t_symbol *sym, long argc, t_atom *argv)
{
    FFT_SETUP_D fft_setup;

    FFT_SPLIT_COMPLEX_D spectrum;

    float *amp_vals;
    float *phase_vals;
    float *in_buf;

    double *temp_d;

    double sample_rate;
    double start_ms = 0;
    double end_ms = 0;

    AH_UIntPtr fft_size_log2;
    AH_UIntPtr fft_size;

    AH_SIntPtr length;
    AH_SIntPtr offset;
    AH_SIntPtr i;

    t_symbol *buffer;
    t_atom_long display = 0;

    t_atom_long read_chan = x->read_chan - 1;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for extract message");
        return;
    }

    buffer = atom_getsym(argv);

    if (argc > 1)
        display = atom_getlong(argv + 1);
    if (argc > 2)
        start_ms = atom_getfloat(argv + 2);
    if (argc > 3)
        end_ms = atom_getfloat(argv + 3);

    // Check input buffer

    if (buffer_check((t_object *) x, buffer))
        return;

    length = buffer_length(buffer);
    sample_rate = buffer_sample_rate(buffer);

    // Check curve range

    if (display < 0)
        display = 0;
    if (display > 4)
        display = 4;
    if (display)
        display--;

    if (!end_ms)
    {
        end_ms = start_ms;
        start_ms = 0;
    }

    if (start_ms < 0)
        start_ms = 0;

    if (end_ms < start_ms)
        return;

    if (!end_ms && !start_ms)
        end_ms = HUGE_VAL;

    start_ms *= sample_rate / 1000.0;
    end_ms *= sample_rate / 1000.0;

    offset = (AH_SIntPtr) start_ms;

    if (length > end_ms)
        length = (AH_SIntPtr) end_ms;

    length -= offset;

    if (length > 0)
    {
        // Calculate FFT size

        fft_size = calculate_fft_size(length, &fft_size_log2);

        // Allocate storage memory

        amp_vals = (float *) schedule_equal_mem_swap(&x->curve_data[display], (fft_size + 2) * sizeof(float), fft_size);
        phase_vals = amp_vals + (fft_size >> 1) + 1;

        if (!amp_vals)
        {
            object_error((t_object *) x, "could not allocate internal memory for data storage");
            return;
        }

        // Allocate and check temporary memory

        in_buf = allocate_aligned<float>(fft_size);
        temp_d = allocate_aligned<double>(fft_size);
        hisstools_create_setup(&fft_setup, fft_size_log2);

        if (!fft_setup || !in_buf || !temp_d)
        {
            hisstools_destroy_setup(fft_setup);
            deallocate_aligned(in_buf);
            deallocate_aligned(temp_d);
            object_error((t_object *) x, "could not allocate internal memory for analysis");
            return;
        }

        spectrum.realp = (double *) (in_buf + fft_size);
        spectrum.imagp = spectrum.realp + (fft_size >> 1);

        // Read buffer and do FFT

        buffer_read_part(buffer, read_chan, in_buf, offset, length);
        time_to_halfspectrum_float(fft_setup, in_buf, length, spectrum, fft_size);

        // Calculate Powers and Phases

        temp_d[0] = spectrum.realp[0] * spectrum.realp[0];
        phase_vals[0] = 0.0;

        for (i = 1; i < (AH_SIntPtr) (fft_size >> 1); i++)
        {
            temp_d[i] = (spectrum.realp[i] * spectrum.realp[i]) + (spectrum.imagp[i] * spectrum.imagp[i]);
            phase_vals[i] = atan2f((float) spectrum.imagp[i], (float) spectrum.realp[i]);
        }

        temp_d[i] = spectrum.realp[i] * spectrum.realp[i];
        phase_vals[i] = 0;

        spectrumdraw_octave_smooth(temp_d, amp_vals, (fft_size >> 1) + 1, x->oct_smooth);

        x->curve_sr[display] = sample_rate;
        x->curve_freeze[display] = 2;

        // Free memory

        hisstools_destroy_setup(fft_setup);
        deallocate_aligned(in_buf);
        deallocate_aligned(temp_d);

        // Redraw

        spectrumdraw_calc_selection_data(x);
        jbox_redraw((t_jbox *) x);
    }
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Window Generation ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_generate_window(t_spectrumdraw *x, AH_UIntPtr window_size, AH_UIntPtr fft_size)
{
    t_window_type window_type = (t_window_type) x->window_type;

    AH_UIntPtr mem_size;
    AH_UIntPtr i, j;

    float *window = (float *) access_mem_swap(&x->realtime_temp, &mem_size);

    float scale;

    double gain = 0.0;
    double alpha_bessel_recip;
    double new_term;
    double x_sq;
    double b_func;
    double temp;

    if (mem_size != fft_size)
        return;

    switch (window_type)
    {
        case WIND_RECT:
            for (i = 0; i < window_size; i++)
                window[i] = 1.f;
            break;

        case WIND_HANN:
            for (i = 0; i < window_size; i++)
                window[i] = (float) (0.5 - (0.5 * cos(FFTW_TWOPI * ((double) i / (double) window_size))));
            break;

        case WIND_HAMMING:
            for (i = 0; i < window_size; i++)
                window[i] = (float) (0.54347826 - (0.45652174 * cos(FFTW_TWOPI * ((double) i / (double) window_size))));
            break;

        case WIND_KAISER:

            // First find bessel function of alpha

            x_sq = 46.24;
            new_term = 0.25 * x_sq;
            b_func = 1.0;
            j = 2;

            // Gives maximum accuracy

            while (new_term)
            {
                b_func += new_term;
                new_term = new_term * x_sq * (1.0 / (4.0 * (double) j * (double) j));
                j++;
            }

            alpha_bessel_recip = 1.0 / b_func;

            // Now create kaiser window

            for (i = 0; i < window_size; i++)
            {
                temp = ((2.0 * (double) i) - ((double) window_size - 1.0));
                temp = temp / window_size;
                temp *= temp;
                x_sq = (1 - temp) * 46.24;
                new_term = 0.25 * x_sq;
                b_func = 1;
                j = 2;

                // Gives maximum accuracy

                while (new_term)
                {
                    b_func += new_term;
                    new_term = new_term * x_sq * (1.0 / (4.0 * (double) j * (double) j));
                    j++;
                }
                window[i] = (float) (b_func * alpha_bessel_recip);
            }
            break;

        case WIND_TRIANGLE:
            for (i = 0; i < (window_size >> 1); i++)
                window[i] = (float) ((double) i / (double) (window_size >> 1));
            for (; i < window_size; i++)
                window[i] = (float) ((double) (((double) window_size - 1) - (double) i) / (double) (window_size >> 1));
            break;

        case WIND_BLACKMAN:
            for (i = 0; i < window_size; i++)
                window[i] = (float) (0.42659071 - (0.49656062 * cos(FFTW_TWOPI * ((double) i / (double) window_size))) + (0.07684867 * cos(FFTW_FOURPI * ((double) i / (double) window_size))));
            break;

        case WIND_BLACKMAN_62:
            for (i = 0; i < window_size; i++)
                window[i] = (float) ((0.44859f - 0.49364f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.05677f * cos(FFTW_FOURPI * ((double) i / (double) window_size))));
            break;

        case WIND_BLACKMAN_70:
            for (i = 0; i < window_size; i++)
                window[i] = (float) ((0.42323f - 0.49755f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.07922f * cos(FFTW_FOURPI * ((double) i / (double) window_size))));
            break;

        case WIND_BLACKMAN_74:
            for (i = 0; i < window_size; i++)
                window[i] = (float) ((0.402217f - 0.49703f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.09892f * cos(FFTW_FOURPI * ((double) i / (double) window_size)) - 0.00188 * cos(FFTW_THREEPI * ((double) i / (double) window_size))));
            break;

        case WIND_BLACKMAN_92:
            for (i = 0; i < window_size; i++)
                window[i] = (float) ((0.35875f - 0.48829f * cos(FFTW_TWOPI * ((double) i / (double) window_size)) + 0.14128f * cos(FFTW_FOURPI * ((double) i / (double) window_size)) - 0.01168 * cos(FFTW_THREEPI * ((double) i / (double) window_size))));
            break;

        case WIND_BLACKMAN_HARRIS:
            for (i = 0; i < window_size; i++)
                window[i] = (float) (0.35875 - (0.48829 * cos(FFTW_TWOPI * ((double) i / (double) window_size))) + (0.14128 * cos(FFTW_FOURPI * ((double) i / (double) window_size))) - (0.01168 * cos(FFTW_SIXPI * ((double) i / (double) window_size))));
            break;

        case WIND_FLAT_TOP:
            for (i = 0; i < window_size; i++)
                window[i] = (float) (0.2810639 - (0.5208972 * cos(FFTW_TWOPI * ((double) i / (double) window_size))) + (0.1980399 * cos(FFTW_FOURPI * ((double) i / (double) window_size))));
            break;
    }

    // Calculate the gain of the window

    for (i = 0, gain = 0; i < window_size; i++)
        gain += (double) window[i];

    // N.B. Multiply by 2 due to real only signal (real sine wave has +/-ve frequencies of 1/2 amplitude each)

    scale = 2.f / ((float) gain);

    for (i = 0; i < window_size; i++)
        window[i] *= scale;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Realtime Analysis ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_realtime(t_spectrumdraw *x, float *read_from, long phase_mode, long N)
{
    FFT_SPLIT_COMPLEX_D spectrum;
    FFT_SETUP_D fft_setup;

    AH_UIntPtr fft_size = (AH_UIntPtr) 1 << (x->fft_select + 8);
    AH_UIntPtr window_size = fft_size / ((AH_UIntPtr) 1 << x->zero_pad);
    AH_UIntPtr i, j;

    float *amp_vals;
    float *phase_vals;
    float *in_window;
    float *in_temp_f;
    double *in_temp_d;

    t_frame_stats *frame_stats;
    long curve_mode;

    // Attempt to swap memory

    if ((attempt_mem_swap(&x->realtime_temp) == SWAP_FAILED) || (attempt_mem_swap(&x->realtime_setup) == SWAP_FAILED))
        return;

    if ((x->realtime_temp.current_size != fft_size) || (x->realtime_setup.current_size != fft_size))
        return;

    // Set pointers

    in_window = (float *) x->realtime_temp.current_ptr;

    spectrum.realp = (double *) (in_window + fft_size);
    spectrum.imagp = spectrum.realp + (fft_size >> 1);

    in_temp_f = (float *) (spectrum.imagp + (fft_size >> 1));
    in_temp_d = (double *) (in_temp_f + fft_size + 1);

    fft_setup = (FFT_SETUP_D) x->realtime_setup.current_ptr;

    // Window and zero pad

    for (i = 0; i < window_size; i++)
        in_temp_f[i] = read_from[i] * in_window[i];
    for (; i < fft_size; i++)
         in_temp_f[i] = 0.f;

    // Do FFT

    time_to_halfspectrum_float(fft_setup, in_temp_f, fft_size, spectrum, fft_size);

    // Calculate power values

    in_temp_d[0] = spectrum.realp[0] * spectrum.realp[0];

    for (i = 1; i < fft_size >> 1; i++)
        in_temp_d[i] = ((spectrum.realp[i] * spectrum.realp[i]) + (spectrum.imagp[i] * spectrum.imagp[i]));

    in_temp_d[i] = spectrum.realp[i] * spectrum.realp[i];

    // Smooth

    spectrumdraw_octave_smooth(in_temp_d, in_temp_f, (fft_size >> 1) + 1, x->oct_smooth);

    // Do Stats etc. Per Curve

    for (j = 0; j < SPECTRUMDRAW_NUM_CURVES; j++)
    {
        curve_mode = x->curve_mode[j];

        if ((x->curve_chan[j] - 1) != N || curve_mode == 0)
            continue;

        if ((attempt_mem_swap(&x->realtime_data[j]) == SWAP_FAILED) || (attempt_mem_swap(&x->realtime_stats[j]) == SWAP_FAILED))
            continue;

        if ((x->realtime_data[j].current_size != fft_size)  || (x->realtime_stats[j].current_size != fft_size))
            continue;

        amp_vals = (float *) x->realtime_data[j].current_ptr;
        phase_vals = amp_vals + (fft_size >> 1) + 1;

        // Update frame stats parameters

        frame_stats = (t_frame_stats *) x->realtime_stats[j].current_ptr;

        frame_stats_mode(frame_stats, (t_frame_mode) (curve_mode - 1));
        frame_stats_max_age(frame_stats, (AH_UInt32) ceil(x->peak_hold / x->redraw_time));
        frame_stats_alpha(frame_stats, 2 / ((x->time_smooth[0] / x->redraw_time) + 1.0), 2 / ((x->time_smooth[1] / x->redraw_time) + 1.0));

        // Do stats

        frame_stats_write(frame_stats, in_temp_f, (fft_size >> 1) + 1);

        // Check freeze status

        if (x->curve_freeze[j])
            continue;

        // Store amps

        frame_stats_read(frame_stats, amp_vals, (fft_size >> 1) + 1);

        if (phase_mode)
        {
            // Store phases (if neeeded)

            phase_vals[0] = 0.0;

            for (i = 1; i < (fft_size >> 1); i++)
                phase_vals[i] = atan2f((float) spectrum.imagp[i], (float) spectrum.realp[i]);

            phase_vals[i] = 0.0;
        }
        else
        {
            // Zero phases

            for (i = 0; i < (fft_size >> 1) + 1; i++)
                phase_vals[i] = 0.0;
        }
    }

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Perform Routines ////////////////////////////
//////////////////////////////////////////////////////////////////////////


t_int *spectrumdraw_perform(t_int *w)
{
    // Set pointers

    float **sig_ins = (float **) w[1];
    long vec_size = (long) w[2];
    t_spectrumdraw *x = (t_spectrumdraw *) w[3];

    float *sig_in;
    float *in_store;

    long write_pointer = x->write_pointer;
    long hop_pointer = x->hop_pointer;
    long block_write_pointer = write_pointer;
    long block_hop_pointer = hop_pointer;
    long window_size = (1 << (x->fft_select + 8)) / (1 << x->zero_pad);
    long hop_size = (long) (x->redraw_time * x->sample_rate / 1000.0);
    long draw = 0;
    long i, j;

    for (j = 0; j < SPECTRUMDRAW_NUM_CURVES; j++)
    {
        if (attempt_mem_swap(x->realtime_io + j) == SWAP_FAILED)
            continue;

        in_store = (float *) x->realtime_io[j].current_ptr;
        sig_in = sig_ins[j];

        if ((long) x->realtime_io[j].current_size < window_size || !in_store || !sig_in)
            continue;

        write_pointer = block_write_pointer;
        hop_pointer = block_hop_pointer;

        for (i = 0; i < vec_size; i++)
        {
            if (hop_pointer >= hop_size)
            {
                // Add a threadsafe mechanism here...

                if (ATOMIC_INCREMENT(&x->draw_check) == 1)
                {
                    spectrumdraw_realtime(x, in_store + write_pointer, x->phase_mode, j);
                    draw = 1;
                }
                ATOMIC_DECREMENT(&x->draw_check);
                hop_pointer = 0;
            }

            if (write_pointer >= window_size)
                write_pointer = 0;

            in_store[write_pointer] = in_store[write_pointer + window_size] = *sig_in++;
            write_pointer++;
            hop_pointer++;
        }
    }

    x->write_pointer = write_pointer;
    x->hop_pointer = hop_pointer;

    // Only draw once per signal vector

    if (draw)
        jbox_redraw((t_jbox *)x);

    return w + 4;
}


void spectrumdraw_perform64(t_spectrumdraw *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *sig_in;
    float *in_store;

    long write_pointer = x->write_pointer;
    long hop_pointer = x->hop_pointer;
    long block_write_pointer = write_pointer;
    long block_hop_pointer = hop_pointer;
    long window_size = (1 << (x->fft_select + 8)) / (1 << x->zero_pad);
    long hop_size = (long) (x->redraw_time * x->sample_rate / 1000.0);
    long draw = 0;
    long i, j;

    for (j = 0; j < SPECTRUMDRAW_NUM_CURVES; j++)
    {
        if (attempt_mem_swap(x->realtime_io + j) == SWAP_FAILED)
            continue;

        in_store = (float *) x->realtime_io[j].current_ptr;
        sig_in = ins[j];

        if ((long) x->realtime_io[j].current_size < window_size || !in_store || !sig_in)
            continue;

        write_pointer = block_write_pointer;
        hop_pointer = block_hop_pointer;

        for (i = 0; i < vec_size; i++)
        {
            if (hop_pointer >= hop_size)
            {
                // Add a threadsafe mechanism here...

                if (ATOMIC_INCREMENT(&x->draw_check) == 1)
                {
                    spectrumdraw_realtime(x, in_store + write_pointer, x->phase_mode, j);
                    draw = 1;
                }
                ATOMIC_DECREMENT(&x->draw_check);
                hop_pointer = 0;
            }

            if (write_pointer >= window_size)
                write_pointer = 0;

            in_store[write_pointer] = in_store[write_pointer + window_size] = (float) *sig_in++;
            write_pointer++;
            hop_pointer++;
        }
    }

    x->write_pointer = write_pointer;
    x->hop_pointer = hop_pointer;

    // Only draw once per signal vector

    if (draw)
        jbox_redraw((t_jbox *)x);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// DSP Routines ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_dsp_common(t_spectrumdraw *x, double samplerate, t_signal **sp, short *count)
{
    short i;

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
    {
        if (count[i] > 0)
        {
            x->sig_ins_valid[i] = 1;
            if (sp)
                x->sig_ins[i] = sp[i]->s_vec;
        }
        else
        {
            x->sig_ins[i] = 0;
            x->sig_ins_valid[i] = 0;
        }
    }

    x->sample_rate = samplerate;
    check_realtime_io(x, (AH_UIntPtr) 1 <<(x->fft_select + 8));
}


void spectrumdraw_dsp(t_spectrumdraw *x, t_signal **sp, short *count)
{
    spectrumdraw_dsp_common(x, sp[0]->s_sr, sp, count);
    dsp_add((t_perfroutine)spectrumdraw_perform, 3, x->sig_ins, sp[0]->s_n, x);
}


void spectrumdraw_dsp64(t_spectrumdraw *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    spectrumdraw_dsp_common(x, samplerate, 0, count);
    object_method(dsp64, gensym("dsp_add64"), x, spectrumdraw_perform64, 0, NULL);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Label Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////


long gcd(long a, long b)
{
    long c;

    if (a < 0)
        a = -a;

    if (a == b || a == 0 || b == 0)
        return b;

    if (a < b)
    {
        c = a;
        a = b;
        b = c;
    }

    while(1)
    {
        c = a % b;

        if (c == 0)
            return b;
        a = b;
        b = c;
    }
}


void phase_label(char *text, long n, long d)
{
    char n_text[32];
    char d_text[32];

    long c = gcd(n, d);

    if (c)
    {
        n /= c;
        d /= c;
    }

    if (n > 1 || n < -1)
        sprintf(n_text,"%+ld", n);
    else
    {
        if (n == 1)
            sprintf(n_text, "+");
        else
            sprintf(n_text, "-");
    }

    if (d > 1)
        sprintf(d_text,"/%ld", d);
    else
        d_text[0] = 0;

    if (n != 0)
        sprintf(text,"%sπ%s", n_text, d_text);
    else
        sprintf(text,"0");
}


void amp_label(char *text, double amp, long amp_grid, int base_precision)
{
    int precision = amp_precisions[amp_grid];

    precision = base_precision > precision ?  base_precision : precision;

    sprintf(text,"%.*lf", precision, amp);
}


void frequency_label(char *text, double freq)
{
    int i;
    int precision = 0;

    if (freq / 1000.0 >= 0.9998)
    {

        freq = round(freq) / 1000.0;

        if ((freq - floor(freq)) * 10.0 > 0.9998)
            precision = 1;

        sprintf(text,"%.*lfk", precision, freq);
    }
    else
    {
        if (freq < 1.0)
            precision = 1;
        if (freq < 0.5)
            precision = 2;
        if (freq < 0.25)
            precision = 3;

        for (i = 0; i < precision; i++)
            freq *= 10.0;

        freq = round (freq);

        for (i = 0; i < precision; i++)
            freq /= 10.0;

        sprintf(text,"%.*lf", precision, freq);
    }
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Paint Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void spectrumdraw_jgraphics_paint_boxes(t_spectrumdraw *x, t_jgraphics *g, double width, double height, double sel_min_freq, double sel_max_freq, t_scale_vals *scale)
{
    // Clear

    jgraphics_set_source_jrgba(g, &x->u_background);
    jgraphics_rectangle(g, 0, 0, width, height);
    jgraphics_fill(g);

    // Do Selection

    if (x->selection_on)
    {
        double x_from = freq_2_mouse(sel_min_freq, scale);
        double x_width = freq_2_mouse(sel_max_freq, scale) - x_from;

        jgraphics_set_source_jrgba(g, &x->u_select);
        jgraphics_rectangle(g, x_from, 0, x_width, height);
        jgraphics_fill(g);
    }
}


void spectrumdraw_jgraphics_paint_grid(t_spectrumdraw *x, t_jgraphics *g, double width, double height, t_scale_vals *scale)
{
    // Do grid

    t_jrgba grid = x->u_grid;

    double freq_ref = scale->freq_ref;
    double grid_mul = scale->freq_mul;
    double x_ref = freq_2_mouse(freq_ref, scale);

    double y_space = scale->y_space;
    double y_ref = scale->y_ref;

    jgraphics_set_source_jrgba(g, &grid);

    if (x->freq_grid)
    {
        switch (x->grid_style)
        {
            case 0:

                for (; x_ref < width; freq_ref *= grid_mul, x_ref = freq_2_mouse(freq_ref, scale))
                    jgraphics_line_draw_fast (g, round(x_ref) + 0.5, 0, round(x_ref) + 0.5, height, 1);
                break;

            default:

                for (; x_ref < width; freq_ref *= (grid_mul * grid_mul), x_ref = freq_2_mouse(freq_ref, scale))
                {
                    jgraphics_rectangle(g, x_ref, 0, freq_2_mouse(freq_ref * grid_mul, scale) - x_ref, height);
                    jgraphics_fill(g);
                }
                break;
        }
    }

    if (x->amp_grid)
    {
        switch (x->grid_style)
        {
            case 0:

                for (y_ref = y_ref - (ceil(y_ref / y_space) * y_space); y_ref < height; y_ref += y_space)
                    jgraphics_line_draw_fast (g, 0, round(y_ref) + 0.5, width, round(y_ref) + 0.5, 1);
                break;

            default:

                for (y_ref = y_ref - (ceil(y_ref / y_space) * y_space) ;y_ref < height; y_ref += 2 * y_space)
                {
                    jgraphics_rectangle(g, 0, y_ref, width, y_space);
                    jgraphics_fill(g);
                }
                break;
        }
    }
}


void spectrumdraw_jgraphics_paint_ticks(t_spectrumdraw *x, t_jgraphics *g, double width, double height, t_scale_vals *scale)
{
    // Do ticks

    t_jrgba tick = x->u_tick;

    double tick_length = 5.0;

    double freq_ref = scale->freq_ref;
    double grid_mul = scale->freq_mul;
    double x_ref = freq_2_mouse(freq_ref, scale);

    double y_space = scale->y_space;
    double y_ref = scale->y_ref;

    jgraphics_set_source_jrgba(g, &tick);

    if (x->freq_grid)
    {
        for (; x_ref < width; freq_ref *= grid_mul, x_ref = freq_2_mouse(freq_ref, scale))
        {
            jgraphics_line_draw_fast(g, x_ref, 0, x_ref, tick_length, 1);
            jgraphics_line_draw_fast(g, x_ref, height - tick_length, x_ref, height, 1);
        }
    }

    if (x->amp_grid)
    {
        for (y_ref = y_ref - (ceil(y_ref / y_space) * y_space) ;y_ref < height; y_ref += y_space)
        {
            jgraphics_line_draw_fast(g, 0, y_ref, tick_length, y_ref, 1);
            jgraphics_line_draw_fast(g, width - tick_length, y_ref, width, y_ref, 1);
        }
    }
}


void spectrumdraw_jgraphics_paint_markers(t_spectrumdraw *x, t_jgraphics *g, double width, double height, t_scale_vals *scale)
{
    // Do Markers

    double *markers = x->markers;
    long i;

    jgraphics_set_source_jrgba(g, &x->u_marker);

    for (i = 0; i < x->num_markers; i++)
    {
        double x_val = freq_2_mouse(markers[i], scale);
        jgraphics_line_draw_fast(g, x_val, 0, x_val, height, 1);
    }
}


void spectrumdraw_jgraphics_curve_vertex (t_jgraphics *g, double x, double y, t_curve_style style)
{
    switch (style)
    {
        case DRAW_LINES:

            jgraphics_line_to(g, x, y);
            jgraphics_stroke(g);
            jgraphics_move_to(g, x, y);
            break;

        case DRAW_POINTS:

            jgraphics_ellipse (g, x - 1.35, y - 1.35, 2.7, 2.7);
            jgraphics_fill(g);
            break;

        case DRAW_POLY:
        default:

            jgraphics_line_to(g, x, y);
            break;
    }
}


void spectrumdraw_jgraphics_paint_curve_internal(t_spectrumdraw *x, t_jgraphics *g, AH_UIntPtr from, AH_UIntPtr to, float *y_vals, t_scale_vals *scale, double zoom_factor, double sub_sample_render, double width, double height, t_jrgba *color, t_curve_style curve_style, t_subsample_style sub_sample_style)
{
    // Do Curve

    double last_x_pos;
    double x_pos;
    double last_x_draw;

    AH_UIntPtr i;

    jgraphics_set_source_jrgba(g, color);

    from = !from ? 1 : from;
    x_pos = bin_2_mouse((double) from, scale);

    if (x_pos > 0.0 && curve_style != DRAW_POINTS)
    {
        jgraphics_move_to (g, 0, yval_2_mouse(y_vals[from], scale));
        spectrumdraw_jgraphics_curve_vertex(g, x_pos, yval_2_mouse(y_vals[from], scale), curve_style);
    }
    else
        jgraphics_move_to(g, x_pos, yval_2_mouse(y_vals[from], scale));

    for (i = from + 1; i < to; i++)
    {
        last_x_pos = x_pos;
        x_pos = bin_2_mouse((double) i, scale);

        spectrumdraw_jgraphics_curve_vertex(g, x_pos, yval_2_mouse(y_vals[i], scale), curve_style);

        if ((x_pos - last_x_pos) < sub_sample_render)
            break;
    }

    last_x_draw = x_pos;

    switch (sub_sample_style)
    {
        case SUBSAMPLE_FULL:

            while (i < to)
            {
                double min_y = y_vals[i];
                double max_y = y_vals[i];

                double x_min_pos = x_pos;
                double x_max_pos = x_pos;

                last_x_pos = x_pos;
                x_pos = bin_2_mouse((double) ++i, scale);

                while (((x_pos - last_x_pos) < sub_sample_render) && i < to)
                {
                    double test_val = y_vals[i];

                    if (test_val < min_y)
                    {
                        min_y = test_val;
                        x_min_pos = x_pos;
                    }

                    if (test_val > max_y)
                    {
                        max_y = test_val;
                        x_max_pos = x_pos;
                    }

                    x_pos = bin_2_mouse((double) ++i, scale);
                }

                if (x_min_pos < x_max_pos)
                {
                    min_y = yval_2_mouse(min_y, scale);
                    max_y = yval_2_mouse(max_y, scale);

                    spectrumdraw_jgraphics_curve_vertex(g, x_min_pos, min_y, curve_style);
                    spectrumdraw_jgraphics_curve_vertex(g, x_max_pos, max_y, curve_style);

                    last_x_draw = x_max_pos;
                }
                else
                {
                    min_y = yval_2_mouse(min_y, scale);
                    max_y = yval_2_mouse(max_y, scale);

                    spectrumdraw_jgraphics_curve_vertex(g, x_max_pos, max_y, curve_style);
                    spectrumdraw_jgraphics_curve_vertex(g, x_min_pos, min_y, curve_style);

                    last_x_draw = x_min_pos;
                }
            }
            break;

        case SUBSAMPLE_PEAK:

            while (i < to)
            {
                double max_y = y_vals[i];
                double x_max_pos = x_pos;

                last_x_pos = x_pos;
                x_pos = bin_2_mouse((double) ++i, scale);

                while (((x_pos - last_x_pos) < sub_sample_render) && i < to)
                {
                    double test_val = y_vals[i];

                    if (test_val > max_y)
                    {
                        max_y = test_val;
                        x_max_pos = x_pos;
                    }

                    x_pos = bin_2_mouse((double) ++i, scale);
                }

                max_y = yval_2_mouse(max_y, scale);
                spectrumdraw_jgraphics_curve_vertex(g, x_max_pos, max_y, curve_style);

                last_x_draw = x_pos;
            }
            break;

        case SUBSAMPLE_AVERAGE:

            while (i < to)
            {
                double accum_val = y_vals[i];
                AH_UIntPtr count = 1;

                last_x_pos = x_pos;
                x_pos = bin_2_mouse((double) ++i, scale);

                while (((x_pos - last_x_pos) < sub_sample_render) && i < to)
                {
                    accum_val += y_vals[i];
                    x_pos = bin_2_mouse((double) ++i, scale);
                    count++;
                }

                accum_val = yval_2_mouse(accum_val / count, scale);
                spectrumdraw_jgraphics_curve_vertex(g, x_pos, accum_val, curve_style);

                last_x_draw = x_pos;
            }
            break;
    }

    if (curve_style == DRAW_POLY)
    {
        jgraphics_line_to(g, last_x_draw, height);
        jgraphics_line_to(g, 0, height);
        jgraphics_close_path(g);
        jgraphics_fill(g);
    }
}


void spectrumdraw_jgraphics_paint_curve(t_spectrumdraw *x, t_jgraphics *g, AH_UIntPtr from, AH_UIntPtr to, float *y_vals, t_scale_vals *scale, double zoom_factor, double sub_sample_render, double width, double height, t_jrgba *color)
{
    t_curve_style curve_style = (t_curve_style) x->curve_style;
    t_subsample_style subsample_style = x->phase_mode ? SUBSAMPLE_FULL : (t_subsample_style) x->subsample_style;

    if (curve_style == DRAW_LINE_POLY)
    {
        spectrumdraw_jgraphics_paint_curve_internal (x, g, from, to, y_vals, scale, zoom_factor, sub_sample_render, width, height, color, DRAW_POLY, subsample_style);
        curve_style = DRAW_LINES;
    }

    spectrumdraw_jgraphics_paint_curve_internal (x, g, from, to, y_vals, scale, zoom_factor, sub_sample_render, width, height, color, curve_style, subsample_style);
}


void spectrumdraw_jgraphics_paint(t_spectrumdraw *x, t_object *patcherview, t_scale_vals *scale, double zoom_factor, double sub_sample_render, t_rect rect, double sel_min_freq, double sel_max_freq)
{
    t_jgraphics *g = jbox_start_layer((t_object *) x, patcherview, gensym("background_layer"), rect.width, rect.height);
    t_jgraphics *g2;

    float *y_vals;
    short i;

    jbox_get_color((t_object *) x, &x->curve_colors[0]);

    if (g)
    {
        spectrumdraw_jgraphics_paint_boxes(x, g, rect.width, rect.height, sel_min_freq, sel_max_freq, scale);
        if (x->grid_style != 2)
            spectrumdraw_jgraphics_paint_grid(x, g, rect.width, rect.height, scale);
        spectrumdraw_jgraphics_paint_markers(x, g, rect.width, rect.height, scale);
        jbox_end_layer((t_object*)x, patcherview, gensym("background_layer"));
    }
    jbox_paint_layer((t_object *) x, patcherview, gensym("background_layer"), rect.x, rect.y);

    jbox_invalidate_layer((t_object*)x, NULL, gensym("curve_layer"));
    g2 = jbox_start_layer((t_object *) x, patcherview, gensym("curve_layer"), rect.width, rect.height);

    if (g2)
    {
        for (i = SPECTRUMDRAW_NUM_CURVES - 1; i >= 0; i--)
        {
            if (x->curve_data[i].current_ptr)
            {
                if (x->phase_mode)
                    y_vals = ((float *) x->curve_data[i].current_ptr) + (x->curve_data[i].current_size >> 1) + 1;
                else
                    y_vals = (float *) x->curve_data[i].current_ptr;

                jgraphics_set_line_width(g2, x->curve_thickness[i]);
                spectrumdraw_set_fft_scaling(scale, x->curve_data[i].current_size, x->curve_sr[i], 1.0);
                spectrumdraw_jgraphics_paint_curve (x, g2, scale->from, scale->to, y_vals, scale, zoom_factor, sub_sample_render, rect.width, rect.height, x->curve_colors + i);
                jgraphics_set_line_width(g2, 1.0);
            }
        }
        if (x->grid_style > 1)
            spectrumdraw_jgraphics_paint_ticks(x, g2, rect.width, rect.height, scale);
        jbox_end_layer((t_object*)x, patcherview, gensym("curve_layer"));
    }
    jbox_paint_layer((t_object *) x, patcherview, gensym("curve_layer"), rect.x, rect.y);
}


void spectrumdraw_paint_labels(t_spectrumdraw *x, t_jgraphics *g, t_scale_vals *scale, double x_offset, double y_offset, double width, double height, double label_width, double label_height, double max_text_width, double max_text_height)
{
    // Do Text

    t_jgraphics_text_justification justify = JGRAPHICS_TEXT_JUSTIFICATION_LEFT;
    t_jtextlayout *jtl;
    t_jfont *jf;

    char text[256];

    double freq_ref = scale->freq_ref;
    double grid_mul = scale->freq_mul;
    double x_ref = freq_2_mouse(freq_ref, scale);
    double y_space = scale->y_space;
    double y_ref = scale->y_ref;
    double last_draw = -10000.0;
    double offset = 0.0;
    double amp_ref = x->amp_ref;
    double text_height_pad = max_text_height;
    double textwidth;
    double textheight;

    int base_precision = 0;

    jf = jfont_create(jbox_get_fontname((t_object *) x)->s_name, (t_jgraphics_font_slant) jbox_get_font_slant((t_object *) x), (t_jgraphics_font_weight) jbox_get_font_weight((t_object *) x), jbox_get_fontsize((t_object *) x));
    jtl = jtextlayout_create();

    jtextlayout_settextcolor(jtl, &x->u_textcolor);

    jgraphics_select_jfont(g, jf);
    jgraphics_set_source_jrgba(g, &x->u_tick);

    if (x->freq_labels && x->freq_grid)
    {
        last_draw = -10000.0;

        switch (x->label_pos)
        {
            case 0:
            case 1:
                justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_TOP, JGRAPHICS_TEXT_JUSTIFICATION_HCENTERED);
                break;

            case 2:
            case 3:
                justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_BOTTOM, JGRAPHICS_TEXT_JUSTIFICATION_HCENTERED);
                break;
        }

        for (; x_ref <= width + 1; freq_ref *= grid_mul, x_ref = freq_2_mouse(freq_ref, scale))
        {
            frequency_label(text, mouse_2_freq(x_ref, scale));
            jfont_text_measure(jf,  text, &textwidth, &textheight);
            jtextlayout_set(jtl, text, jf, x_ref + x_offset - width, 0, 2.0 * width, label_height, justify, t_jgraphics_textlayout_flags(0));

            // Draw it

            if (x_ref  >= -1.0 && (x_ref - last_draw > (textwidth / 2.0) + 2.0) && (x_ref + x_offset - (textwidth / 2.0)) > 0.0 && (x_ref  + x_offset + (textwidth / 2.0) < label_width))
            {
                jtextlayout_draw(jtl, g);
                last_draw = x_ref + (textwidth / 2.0);
            }
        }
    }

    switch (x->label_pos)
    {
        case 0:
        case 2:
            justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_VCENTERED, JGRAPHICS_TEXT_JUSTIFICATION_RIGHT);
            offset = 0;
            break;

        case 1:
        case 3:
            justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_VCENTERED, JGRAPHICS_TEXT_JUSTIFICATION_RIGHT);
            offset = width + (max_text_height * 0.5);
            break;
    }

    if (x->amp_labels && !x->phase_mode && x->amp_grid)
    {
        last_draw = -10000.0;
        amp_ref = amp_ref - floor(amp_ref);

        if (amp_ref > 0.0)
            base_precision = 1;

        for (y_ref = y_ref - (ceil(y_ref / y_space) * y_space); y_ref <= height + 1; y_ref += y_space)
        {
            if (y_ref >= -1.0 && (y_ref - last_draw > text_height_pad) && (y_ref + y_offset - (max_text_height / 2.0)) >= -1.0 && (y_ref + y_offset + (max_text_height / 2.0)) <= label_height)
            {
                amp_label(text,  mouse_2_yval(y_ref, scale), x->amp_grid, base_precision);
                jtextlayout_set(jtl, text, jf, offset, y_ref - max_text_height + y_offset, max_text_width, 2.0 * max_text_height, justify, t_jgraphics_textlayout_flags(0));
                jtextlayout_settextcolor(jtl, &x->u_textcolor);

                // Draw it

                jtextlayout_draw(jtl, g);
                last_draw = y_ref;
            }
        }
    }

    if (x->phase_labels && x->phase_mode && x->phase_grid)
    {
        long denominator = (long) (phase_denominators[x->phase_grid] * phase_label_spacing[x->phase_labels]);
        long count = (long) ceil(y_ref / y_space);

        last_draw = -10000.0;

        for (y_ref = y_ref - (ceil(y_ref / y_space) * y_space); y_ref <= height + 1; y_ref += y_space)
        {
            if (y_ref >= -1.0 && (y_ref - last_draw > text_height_pad) && (y_ref + y_offset - (max_text_height / 2.0)) >= -1.0 && (y_ref + y_offset + (max_text_height / 2.0)) <= label_height)
            {
                phase_label(text, count, denominator);
                jtextlayout_set(jtl, text, jf, offset, y_ref - max_text_height + y_offset, max_text_width, 2.0 * max_text_height, justify, t_jgraphics_textlayout_flags(0));
                jtextlayout_settextcolor(jtl, &x->u_textcolor);

                // Draw it

                jtextlayout_draw(jtl, g);
                last_draw = y_ref;
            }
            count--;
        }
    }

    jtextlayout_destroy(jtl);
    jfont_destroy(jf);
}


void spectrumdraw_get_measurements(t_spectrumdraw *x, double *return_width, double *return_height, t_rect *rect)
{
    // Do Text

    t_jfont *jf;
    char text[256];
    int base_precision = 0;

    double amp_ref = x->amp_ref;
    double textwidth;
    double textheight;
    double min_height;
    double min_width;
    double y_ref;
    double y_space;
    double y_min;
    double y_max;
    double x_1 = 0.0;
    double y_1 = 0.0;
    double y_2 = 0.0;

    long denominator;
    long count;

    sprintf(text, "0123456789kHzdB");

    jf = jfont_create(jbox_get_fontname((t_object *) x)->s_name, (t_jgraphics_font_slant) jbox_get_font_slant((t_object *) x), (t_jgraphics_font_weight) jbox_get_font_weight((t_object *) x), jbox_get_fontsize((t_object *) x));
    jfont_text_measure(jf, text, &textwidth, &textheight);

    min_width = 0.0;
    min_height = textheight;

    if (x->phase_mode)
    {
        if (x->phase_labels && x->phase_grid)
        {
            denominator = (long) (phase_denominators[x->phase_grid] * phase_label_spacing[x->phase_labels]);
            count = (long) ceil(phase_denominators[x->phase_grid] / phase_label_spacing[x->phase_labels]);

            y_ref = 0.0;
            y_space = phase_grid_spacing[x->phase_grid] * phase_label_spacing[x->phase_labels];

            y_min = -M_PI;
            y_max = M_PI;

            for (y_ref = y_ref - (ceil((y_ref - y_min) / y_space) * y_space); y_ref <= y_max ; y_ref += y_space, count--)
            {
                phase_label(text, count, denominator);
                jfont_text_measure(jf,  text, &textwidth, &textheight);
                min_width = textwidth > min_width ? textwidth : min_width;
            }

            x_1 = min_width + (min_height * 0.5);
            y_1 = min_height * 0.5;
        }
    }
    else
    {
        if (x->amp_labels && x->amp_grid)
        {
            amp_ref = amp_ref - floor(amp_ref);

            y_ref = x->amp_ref;
            y_space = amp_grid_spacing[x->amp_grid] * amp_label_spacing[x->amp_labels];

            y_min = (x->amp_range[0] < x->amp_range[1]) ? x->amp_range[0] : x->amp_range[1];
            y_max = (x->amp_range[0] < x->amp_range[1]) ? x->amp_range[1] : x->amp_range[0];

            if (amp_ref > 0.0)
                base_precision = 1;

            for (y_ref = y_ref - (ceil((y_ref - y_min) / y_space) * y_space); y_ref <= y_max ; y_ref += y_space)
            {
                amp_label(text,  y_ref, x->amp_grid, base_precision);
                jfont_text_measure(jf,  text, &textwidth, &textheight);
                min_width = textwidth > min_width ? textwidth : min_width;
            }

            x_1 = min_width + (min_height * 0.5);
            y_1 = min_height * 0.5;
        }
    }

    jfont_destroy(jf);

    if (x->freq_labels && x->freq_grid)
        y_2 = min_height * 1.5;

    rect->x = (x->label_pos == 0 || x->label_pos == 2) ? x_1 : 0.0;
    rect->y = (x->label_pos == 0 || x->label_pos == 1) ? y_2 : y_1;
    rect->width  -= x_1;
    rect->height -= y_1 + y_2;

    rect->width = rect->width;
    rect->height = rect->height;

    *return_width = min_width;
    *return_height = min_height;
}


void spectrumdraw_paint_selection_data(t_spectrumdraw *x, t_jgraphics *g, float *y_vals, t_scale_vals *scale, AH_SIntPtr fft_size, double x_offset, double y_offset, double width, double height, double sel_min_freq, double sel_max_freq)
{
    // Selection + Point Stuff

    t_jgraphics_text_justification justify = JGRAPHICS_TEXT_JUSTIFICATION_LEFT;
    t_jtextlayout *jtl;
    t_jfont *jf;

    char text[256];
    char note[32];

    double sel_avg_amp = 0.0;
    double sel_min_amp = HUGE_VAL;
    double sel_max_amp = -HUGE_VAL;
    double sel_min_amp_freq;
    double sel_max_amp_freq;
    double textwidth = 0.0;
    double textheight;
    double bin_pos;
    double mouse_x = x->mouse_x;
    double mouse_y = x->mouse_y;
    double x_val;
    double y_val;

    AH_SIntPtr read_lo;
    AH_SIntPtr read_hi;
    AH_SIntPtr sel_from = (AH_SIntPtr) ceil(freq_2_bin(sel_min_freq, scale));
    AH_SIntPtr sel_to = (AH_SIntPtr) floor(freq_2_bin(sel_max_freq, scale));
    AH_SIntPtr sel_min = 0;
    AH_SIntPtr sel_max = 0;
    AH_SIntPtr i;

    AH_Boolean inbox = mouse_x >= 0 && mouse_x <= 1 && mouse_y >= 0 && mouse_y <= 1;

    // Create font

    jf = jfont_create(jbox_get_fontname((t_object *) x)->s_name, (t_jgraphics_font_slant) jbox_get_font_slant((t_object *) x), (t_jgraphics_font_weight) jbox_get_font_weight((t_object *) x), jbox_get_fontsize((t_object *) x));

    // Clip drawing

    jgraphics_save(g);
    jgraphics_clip(g, x_offset, y_offset, width, height);

    // Clip mouse positions and convert

    mouse_x = clip(mouse_x, 0, 1);
    mouse_y = clip(mouse_y, 0, 1);

    mouse_x *= width;
    mouse_y *= height;

    if ((x->mouse_over == true && x->mouse_mode && inbox) || x->selection_on)
    {
        if (!x->selection_on)
        {
            x_val = mouse_2_freq(mouse_x, scale);

            ftonote(note, x_val);

            // Should ideally do cubic interpolation of dB vals here!

            if ((x->mouse_mode == 2 || x->mouse_mode == 4))
            {
                bin_pos = freq_2_bin(x_val, scale);
                read_lo = (AH_SIntPtr) floor(bin_pos);
                read_hi = read_lo + 1;

                if (fft_size && bin_pos >= 0.0 && (bin_pos <= (fft_size >> 1)))
                {
                    // Safefy Check

                    if (read_lo > (fft_size >> 1))
                        read_lo = fft_size >> 1;

                    if (read_hi > (fft_size >> 1))
                        read_hi = fft_size >> 1;

                    y_val = y_vals[read_lo] + (bin_pos - read_lo) * (y_vals[read_hi] - y_vals[read_lo]);
                }
                else
                    y_val = 0.0;


                if (!x->phase_mode)
                    y_val = pow_to_db(y_val);
            }
            else
                y_val = mouse_2_yval(mouse_y, scale);

            if (x->mouse_mode == 2 || x->mouse_mode == 4)
            {
                jgraphics_set_source_jrgba(g, &x->u_indicator);
                jgraphics_ellipse(g, x_offset + mouse_x - 2.5, y_offset + conv_yval_2_mouse(y_val, scale) - 2.5, 5, 5);
                jgraphics_fill(g);
            }

            if (x->phase_mode)
            {
                switch (x->freq_display_options)
                {
                    case 0:
                        sprintf(text,"%.1lf Hz %+.1lf rad", x_val, y_val);
                        break;

                    case 1:
                        sprintf(text,"%s %+.1lf rad", note, y_val);
                        break;

                    case 2:
                        sprintf(text,"%.1lf Hz %s %+.1lf rad", x_val, note, y_val);
                        break;
                }
            }
            else
            {
                switch (x->freq_display_options)
                {
                    case 0:
                        sprintf(text,"%.1lf Hz %+.1lf dB", x_val, y_val);
                        break;

                    case 1:
                        sprintf(text,"%s %+.1lf dB", note, y_val);
                        break;

                    case 2:
                        sprintf(text,"%.1lf Hz %s %+.1lf dB", x_val, note, y_val);
                        break;
                }
            }

            jfont_text_measure(jf, text, &textwidth, &textheight);
        }
        else
        {
            if (!fft_size)
                return;

            for (i = sel_from; i < sel_to; i++)
            {
                double test_val = y_vals[i];

                sel_avg_amp += test_val;

                if (test_val > sel_max_amp)
                {
                    sel_max_amp = test_val;
                    sel_max = i;
                }
                if (test_val < sel_min_amp)
                {
                    sel_min_amp = test_val;
                    sel_min = i;
                }
            }

            sel_avg_amp /= (double) (sel_to - sel_from);
            sel_min_amp_freq = bin_2_freq((double) sel_min, scale);
            sel_max_amp_freq = bin_2_freq((double) sel_max, scale);

            if (x->phase_mode)
            {
                double new_textwidth;

                sprintf(text,"Sel: %.1lfHz - %.1lfHz", sel_min_freq, sel_max_freq);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;

                sprintf(text,"Min: %+.1lfrad @ %.1lfHz", sel_min_amp, sel_min_amp_freq);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;

                sprintf(text, "Max: %+.1lfrad @ %.1lfHz", sel_max_amp, sel_max_amp_freq);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;

                sprintf(text,"Avg: %+.1lfrad", sel_avg_amp);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;;

                textheight *= 4;

                sprintf(text,"Sel: %.1lfHz - %.1lfHz\rMin: %+.1lfrad @ %.1lfHz\rMax: %+.1lfrad @ %.1lfHz\rAvg: %+.1lfrad",
                        sel_min_freq, sel_max_freq, sel_min_amp, sel_min_amp_freq, sel_max_amp, sel_max_amp_freq, sel_avg_amp);
            }
            else
            {
                double new_textwidth;

                sprintf(text,"Sel: %.1lfHz - %.1lfHz", sel_min_freq, sel_max_freq);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;

                sprintf(text,"Min: %+.1lfdB @ %.1lfHz", pow_to_db(sel_min_amp), sel_min_amp_freq);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;

                sprintf(text, "Max: %+.1lfdB @ %.1lfHz", pow_to_db(sel_max_amp), sel_max_amp_freq);
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;

                sprintf(text,"Avg: %+.1lfdB", pow_to_db(sel_avg_amp));
                jfont_text_measure(jf, text, &new_textwidth, &textheight);
                if (new_textwidth > textwidth) textwidth = new_textwidth;;

                textheight *= 4;

                sprintf(text,"Sel: %.1lfHz - %.1lfHz\rMin: %+.1lfdB @ %.1lfHz\rMax: %+.1lfdB @ %.1lfHz\rAvg: %+.1lfdB",
                        sel_min_freq, sel_max_freq, pow_to_db(sel_min_amp), sel_min_amp_freq, pow_to_db(sel_max_amp), sel_max_amp_freq, pow_to_db(sel_avg_amp));
            }

            x_val = bin_2_mouse((double) sel_min, scale);
            y_val = yval_2_mouse(sel_min_amp, scale);

            jgraphics_set_source_jrgba(g, &x->u_indicator);
            jgraphics_ellipse(g, x_offset + x_val - 2.5, y_offset + y_val - 2.5, 5, 5);
            jgraphics_fill(g);

            x_val = bin_2_mouse((double) sel_max, scale);
            y_val = yval_2_mouse(sel_max_amp, scale);

            jgraphics_set_source_jrgba(g, &x->u_indicator);
            jgraphics_ellipse(g, x_offset + x_val - 2.5, y_offset + y_val - 2.5, 5, 5);
            jgraphics_fill(g);
        }

        // Create layout

        jtl = jtextlayout_create();

        // text color

        // set layout / color

        justify = JGRAPHICS_TEXT_JUSTIFICATION_LEFT;

        switch (x->mouse_data_pos)
        {
            case 0:
                justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_TOP, JGRAPHICS_TEXT_JUSTIFICATION_LEFT);
                break;

            case 1:
                justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_TOP, JGRAPHICS_TEXT_JUSTIFICATION_RIGHT);
                break;

            case 2:
                justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_BOTTOM, JGRAPHICS_TEXT_JUSTIFICATION_LEFT);
                break;

            case 3:
                justify = combine_flags(JGRAPHICS_TEXT_JUSTIFICATION_BOTTOM, JGRAPHICS_TEXT_JUSTIFICATION_RIGHT);
                break;
        }

        jtextlayout_set(jtl, text, jf, x_offset + 10, y_offset + 10, width - 20, height - 20, justify, t_jgraphics_textlayout_flags(0));
        jtextlayout_settextcolor(jtl, &x->u_displaytextcolor);

        // Draw box

        jgraphics_set_source_jrgba(g, &x->u_textbox);
        jgraphics_rectangle(g, x->mouse_data_pos & 1 ? x_offset + width - textwidth - 12: x_offset + 8, x->mouse_data_pos > 1 ? y_offset + height - textheight - 12 : y_offset + 8, textwidth + 4, textheight + 4);
        jgraphics_fill(g);

        // Draw it

        jtextlayout_draw(jtl, g);
        jtextlayout_destroy(jtl);
        jfont_destroy(jf);
    }

    jgraphics_restore(g);
}


void spectrumdraw_paint(t_spectrumdraw *x, t_object *patcherview)
{
    float *y_vals;
    t_rect rect, textrect;
    t_scale_vals scale;

    double text_width, text_height;
    double y_min, y_max;
    double min_freq, max_freq;
    double sel_min_freq, sel_max_freq;
    double zoom_factor;
    double sub_sample_render;
    double swap;

    short i;

    // Get graphics context and rect for this view

    t_jgraphics *g = (t_jgraphics*) patcherview_get_jgraphics(patcherview);
    jbox_get_rect_for_view((t_object *) x, patcherview, &textrect);

    rect = textrect;

    // Try to swap memory

    for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
        attempt_mem_swap(x->curve_data + i);

    y_vals = (float *) x->curve_data[x->mouse_curve - 1].current_ptr;

    // Get measurements and set scaling

    min_freq = x->freq_range[0];
    max_freq = x->freq_range[1];

    if (x->phase_mode)
    {
        y_vals += (x->curve_data[x->mouse_curve - 1].current_size >> 1) + 1;
        y_min = -M_PI;
        y_max = M_PI;
    }
    else
    {
        y_min = x->amp_range[0];
        y_max = x->amp_range[1];
    }

    spectrumdraw_get_measurements(x, &text_width, &text_height, &rect);
    spectrumdraw_set_scale_vals(&scale, &rect, min_freq, max_freq, y_min, y_max, !x->linear_mode, !x->phase_mode);
    spectrumdraw_grid_precalc(x, &scale, 0);

    // Load Display Parameters

    sel_min_freq = x->mouse_sel_min_freq;
    sel_max_freq = x->mouse_sel_max_freq;

    if (sel_min_freq > sel_max_freq)
    {
        swap = sel_min_freq;
        sel_min_freq = sel_max_freq;
        sel_max_freq = swap;
    }

    zoom_factor = patcherview_get_zoomfactor(patcherview);
    sub_sample_render = 0.5;

    // Draw Background / Grid / Curves

    spectrumdraw_jgraphics_paint(x, patcherview, &scale, zoom_factor, sub_sample_render, rect, sel_min_freq, sel_max_freq);

    // Draw Overlay Data

    spectrumdraw_grid_precalc(x, &scale, 1);
    spectrumdraw_set_fft_scaling(&scale, x->curve_data[x->mouse_curve - 1].current_size, x->curve_sr[x->mouse_curve - 1], 1.0);
    spectrumdraw_paint_selection_data(x, g, y_vals, &scale, x->curve_data[x->mouse_curve - 1].current_size, rect.x, rect.y, rect.width, rect.height, sel_min_freq, sel_max_freq);
    spectrumdraw_paint_labels(x, g,  &scale, rect.x, rect.y, rect.width, rect.height, textrect.width, textrect.height, text_width, text_height);

    // Draw Border

    jgraphics_set_line_width(g, 1);
    jgraphics_set_source_jrgba(g, &x->u_outline);
    jgraphics_rectangle(g, rect.x + 0.5, rect.y + 0.5, rect.width - 1.0, rect.height - 1.0);
    jgraphics_stroke(g);
}


//////////////////////////////////////////////////////////////////////////
//////////////////// Atrribute Notifications Routines ////////////////////
//////////////////////////////////////////////////////////////////////////


static inline t_symbol *get_attribname_symbol(const char *base_string, short N)
{
    char string_test[32];

    if (!N)
        return gensym("mode");
    else
    {
        sprintf(string_test, "%s%d", base_string, N + 1);
        return gensym(string_test);
    }
}


t_max_err spectrumdraw_notify(t_spectrumdraw *x, t_symbol *sym, t_symbol *msg, void *sender, void *data)
{
    t_symbol *attrname;

    AH_UIntPtr fft_size = (AH_SIntPtr) 1 << (x->fft_select + 8);
    AH_UIntPtr window_size = fft_size / ((AH_SIntPtr) 1 << x->zero_pad);
    AH_UIntPtr new_fft_size_log_2;
    AH_UIntPtr new_fft_size;

    short check_io = 0;
    short i;

    // check notification type

    if (msg == gensym("attr_modified"))
    {
        // get attibute name

        attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));    // ask attribute object for name

        if (attrname == gensym("mousemode"))
        {
            if (x->mouse_mode < 3)
                x->selection_on = 0;
        }

        if (attrname == gensym("freqrange"))
        {
            if (x->freq_range[0] < 0.0001)
                x->freq_range[0] = 0.0001;
            if (x->freq_range[1] < 0.0001)
                x->freq_range[1] = 0.0001;
        }

        jbox_invalidate_layer((t_object*)x, NULL, gensym("background_layer"));
        jbox_invalidate_layer((t_object*)x, NULL, gensym("tick_layer"));
        jbox_invalidate_layer((t_object*)x, NULL, gensym("label_layer"));

        if (attrname == gensym("fftsize"))
        {
            new_fft_size_log_2 = x->fft_select + 8;
            new_fft_size = (AH_SIntPtr) 1 << new_fft_size_log_2;

            schedule_equal_mem_swap(&x->realtime_temp, 2 * (new_fft_size + 1) * (sizeof(float) + sizeof(double)), new_fft_size);
            schedule_equal_mem_swap_custom(&x->realtime_setup, (alloc_method) alloc_fft_setup, (free_method) free_fft_setup, new_fft_size_log_2, new_fft_size);

            check_realtime_io(x, new_fft_size);
        }

        for (i = 0; i < SPECTRUMDRAW_NUM_CURVES; i++)
        {
            if (get_attribname_symbol("mode", i) == attrname)
            {
                x->curve_freeze[i] = 0;
                check_io = 1;
            }

            if (get_attribname_symbol("chan", i) == attrname)
                check_io = 1;
        }

        if (check_io)
            check_realtime_io(x, (AH_UIntPtr) 1 << (x->fft_select + 8));

        if (attrname == gensym("fftsize") || attrname == gensym("zeropad") || attrname == gensym("windowtype"))
            spectrumdraw_generate_window(x, window_size, fft_size);

        spectrumdraw_calc_selection_data(x);
        jbox_redraw((t_jbox *)x);
    }

    return 0;
}
