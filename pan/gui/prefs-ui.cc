/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Pan - A Newsreader for Gtk+
 * Copyright (C) 2002-2006  Charles Kerr <charles@rebelbase.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <config.h>
extern "C" {
  #include <glib/gi18n.h>
  #include <gtk/gtk.h>
}
#include <pan/general/debug.h>
#include <pan/general/macros.h>
#include <pan/icons/pan-pixbufs.h>
#include "hig.h"
#include "pad.h"
#include "pan-file-entry.h"
#include "prefs-ui.h"
#include "tango-colors.h"
#include "url.h"
#include "gtk_compat.h"

using namespace pan;

namespace
{
  void delete_prefs_dialog (gpointer castme)
  {
    delete static_cast<PrefsDialog*>(castme);
  }

  void response_cb (GtkDialog * dialog, int, gpointer)
  {
    gtk_widget_destroy (GTK_WIDGET(dialog));
  }

  #define PREFS_KEY "prefs-key"
  #define PREFS_VAL "prefs-val"

  void toggled_cb (GtkToggleButton * toggle, gpointer prefs_gpointer)
  {
    const char * key = (const char*) g_object_get_data (G_OBJECT(toggle), PREFS_KEY);
    if (key)
      static_cast<Prefs*>(prefs_gpointer)->set_flag (key, gtk_toggle_button_get_active(toggle));
  }

  void set_string_from_radio_cb (GtkToggleButton * toggle, gpointer prefs_gpointer)
  {
    const char * key = (const char*) g_object_get_data (G_OBJECT(toggle), PREFS_KEY);
    const char * val = (const char*) g_object_get_data (G_OBJECT(toggle), PREFS_VAL);
    if (key && val && gtk_toggle_button_get_active(toggle))
      static_cast<Prefs*>(prefs_gpointer)->set_string (key, val);
  }

  GtkWidget* new_check_button (const char* mnemonic, const char* key, bool fallback, Prefs& prefs)
  {
    GtkWidget * t = gtk_check_button_new_with_mnemonic (mnemonic);
    g_object_set_data_full (G_OBJECT(t), PREFS_KEY, g_strdup(key), g_free);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(t), prefs.get_flag (key, fallback));
    g_signal_connect (t, "toggled", G_CALLBACK(toggled_cb), &prefs);
    return t;
  }

  GtkWidget* new_layout_radio (GtkWidget* prev, const guint8* line, const char* value, std::string& cur, Prefs& prefs)
  {
    GtkWidget * r = prev==0
      ? gtk_radio_button_new (NULL)
      : gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON(prev));
    GdkPixbuf * pixbuf = gdk_pixbuf_new_from_inline (-1, line, false, 0);
    GtkWidget * image = gtk_image_new_from_pixbuf (pixbuf);
    g_object_unref (pixbuf);
    gtk_container_add (GTK_CONTAINER(r), image);
    g_object_set_data_full (G_OBJECT(r), PREFS_KEY, g_strdup("pane-layout"), g_free);
    g_object_set_data_full (G_OBJECT(r), PREFS_VAL, g_strdup(value), g_free);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(r), cur==value);
    g_signal_connect (r, "toggled", G_CALLBACK(set_string_from_radio_cb), &prefs);
    return r;
  }

  void spin_value_changed_cb( GtkSpinButton *spin, gpointer data)
  {
    const char * key = (const char*) g_object_get_data (G_OBJECT(spin), PREFS_KEY);
    Prefs *prefs = static_cast<Prefs*>(data);
    prefs->set_int(key, gtk_spin_button_get_value_as_int(spin));
  }
  GtkWidget* new_spin_button (const char *key, int low, int high, Prefs &prefs)
  {
    guint tm = prefs.get_int(key, 5 );
    GtkAdjustment *adj = (GtkAdjustment*) gtk_adjustment_new(tm, low, high, 1.0, 1.0, 1.0);
    GtkWidget *w = gtk_spin_button_new( adj, 1.0, 0);
    g_object_set_data_full(G_OBJECT(w), PREFS_KEY, g_strdup(key), g_free);
    g_signal_connect (w, "value_changed", G_CALLBACK(spin_value_changed_cb), &prefs);
    return w;
  }

  GtkWidget* new_orient_radio (GtkWidget* prev, const char* label, const char* value, std::string& cur, Prefs& prefs)
  {
    GtkWidget * r = prev==0
      ? gtk_radio_button_new_with_label (0, label)
      : gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(prev), label);
    g_object_set_data_full (G_OBJECT(r), PREFS_KEY, g_strdup("pane-orient"), g_free);
    g_object_set_data_full (G_OBJECT(r), PREFS_VAL, g_strdup(value), g_free);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(r), cur==value);
    g_signal_connect (r, "toggled", G_CALLBACK(set_string_from_radio_cb), &prefs);
    return r;
  }

  void set_prefs_string_from_editable (GtkEditable * editable, gpointer prefs_gpointer)
  {
    Prefs * prefs (static_cast<Prefs*>(prefs_gpointer));
    const char * key = (const char*) g_object_get_data (G_OBJECT(editable), PREFS_KEY);
    char * val = gtk_editable_get_chars (editable, 0, -1);
    prefs->set_string (key, val);
    g_free (val);
  }

  void maybe_make_widget_visible (GtkComboBox * c, gpointer user_data)
  {
    GtkWidget * w (GTK_WIDGET(user_data));
    GtkWidget * c_parent (gtk_widget_get_parent (GTK_WIDGET(c)));
    GtkWidget * w_parent (gtk_widget_get_parent (GTK_WIDGET(w)));
    GtkTreeModel * model = gtk_combo_box_get_model (c);
    const int n_rows (gtk_tree_model_iter_n_children (model, NULL));
    const bool do_show (gtk_combo_box_get_active(c) == (n_rows-1));
    if (do_show && !w_parent) // add it
    {
      gtk_box_pack_start (GTK_BOX(c_parent), w, true, true, 0);
      gtk_widget_show (w);
      g_object_unref (G_OBJECT(w));
    }
    else if (!do_show && w_parent) // remove it
    {
      g_object_ref (G_OBJECT(w));
      gtk_container_remove (GTK_CONTAINER(w_parent), w);
    }
  }

  void set_prefs_string_from_combo_box_entry (GtkComboBoxText * c, gpointer user_data)
  {
    const char * key = (const char*) g_object_get_data (G_OBJECT(c), PREFS_KEY);
    char * val = gtk_combo_box_text_get_active_text (c);
    static_cast<Prefs*>(user_data)->set_string (key, val);
    g_free (val);
  }

  GtkWidget* editor_new (Prefs& prefs)
  {
    std::set<std::string> editors;
    URL :: get_default_editors (editors);
    const char * key = "editor";
    const std::string editor = prefs.get_string (key, *editors.begin());
    editors.insert (editor);
    GtkWidget * c = gtk_combo_box_text_new_with_entry ();
    g_object_set_data_full (G_OBJECT(c), PREFS_KEY, g_strdup(key), g_free);
    foreach_const (std::set<std::string>, editors, it)
      gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(c), it->c_str());
    gtk_combo_box_set_active (GTK_COMBO_BOX(c),
                              (int)std::distance (editors.begin(), editors.find(editor)));
    g_signal_connect (c, "changed", G_CALLBACK(set_prefs_string_from_combo_box_entry), &prefs);
    return c;
  }

  void set_prefs_string_from_combobox (GtkComboBox * c, gpointer user_data)
  {
    Prefs * prefs (static_cast<Prefs*>(user_data));
    const char * key = (const char*) g_object_get_data (G_OBJECT(c), PREFS_KEY);
    const int column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT(c), "column"));
    const int row (gtk_combo_box_get_active (c));
    GtkTreeModel * m = gtk_combo_box_get_model (c);
    GtkTreeIter i;
    if (gtk_tree_model_iter_nth_child (m, &i, 0, row)) {
      char * val (0);
      gtk_tree_model_get (m, &i, column, &val, -1);
      prefs->set_string (key, val);
      g_free (val);
    }
  }

  GtkWidget* url_handler_new (Prefs& prefs,
                              const char * mode_key,
                              const char * mode_fallback,
                              const char * custom_key,
                              const char * custom_fallback,
                              GtkWidget *& setme_mnemonic_target)
  {
    // build the combo box...
    const std::string mode (prefs.get_string (mode_key, mode_fallback));
    GtkListStore * store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
    const char* strings[5][2] = { { N_("Use GNOME Preferences"), "gnome" },
                                  { N_("Use KDE Preferences"), "kde" },
                                  { N_("Use OS X Preferences"), "mac" },
                                  { N_("Use Windows Preferences"), "windows" },
                                  { N_("Custom Command:"), "custom" } };
    int sel_index (0);
    for (size_t i=0; i<G_N_ELEMENTS(strings); ++i) {
      GtkTreeIter iter;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, _(strings[i][0]), 1, strings[i][1], -1);
      if (mode == strings[i][1])
        sel_index = i;
    }
    GtkWidget * c = gtk_combo_box_new_with_model (GTK_TREE_MODEL(store));
    GtkCellRenderer * renderer (gtk_cell_renderer_text_new ());
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (c), renderer, true);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (c), renderer, "text", 0, NULL);
    gtk_combo_box_set_active (GTK_COMBO_BOX(c), sel_index);
    g_object_set_data_full (G_OBJECT(c), PREFS_KEY, g_strdup(mode_key), g_free);
    g_object_set_data (G_OBJECT(c), "column", GINT_TO_POINTER(1));
    g_signal_connect (c, "changed", G_CALLBACK(set_prefs_string_from_combobox), &prefs);

    // build the custom entry...
    GtkWidget * e = gtk_entry_new ();
    const std::string custom (prefs.get_string (custom_key, custom_fallback));
    gtk_entry_set_text (GTK_ENTRY(e), custom.c_str());
    g_object_set_data_full (G_OBJECT(e), PREFS_KEY, g_strdup(custom_key), g_free);
    g_signal_connect (e, "changed", G_CALLBACK(set_prefs_string_from_editable), &prefs);

    // tie them together...
    g_signal_connect (c, "changed", G_CALLBACK(maybe_make_widget_visible), e);
    GtkWidget * h = gtk_hbox_new (false, PAD);
    gtk_box_pack_start (GTK_BOX(h), c, true, true, 0);
    gtk_box_pack_start (GTK_BOX(h), e, true, true, 0);

    maybe_make_widget_visible (GTK_COMBO_BOX(c), e);
    setme_mnemonic_target = c;
    return h;
  }

  void font_set_cb (GtkFontButton* b, gpointer prefs_gpointer)
  {
    const char * key = (const char*) g_object_get_data (G_OBJECT(b), PREFS_KEY);
    const char * val = gtk_font_button_get_font_name  (b);
    if (key && *key && val && *val)
      static_cast<Prefs*>(prefs_gpointer)->set_string (key, val);
  }

  GtkWidget* new_font_button (const char* key, const char * fallback, Prefs& prefs)
  {
    const std::string val (prefs.get_string (key, fallback));
    GtkWidget * b = gtk_font_button_new ();
    gtk_font_button_set_font_name (GTK_FONT_BUTTON(b), val.c_str());
    g_object_set_data_full (G_OBJECT(b), PREFS_KEY, g_strdup(key), g_free);
    g_signal_connect (b, "font-set", G_CALLBACK(font_set_cb), &prefs);
    return b;
  }

  void color_set_cb (GtkColorButton* b, gpointer prefs_gpointer)
  {
    const char * key = (const char*) g_object_get_data (G_OBJECT(b), PREFS_KEY);
    GdkColor val;
    gtk_color_button_get_color (b, &val);
    if (key && *key)
      static_cast<Prefs*>(prefs_gpointer)->set_color (key, val);
  }

  GtkWidget* new_color_button (const char* key, const char * fallback, Prefs& prefs)
  {
    const GdkColor val (prefs.get_color (key, fallback));
    GtkWidget * b = gtk_color_button_new_with_color (&val);
    g_object_set_data_full (G_OBJECT(b), PREFS_KEY, g_strdup(key), g_free);
    g_signal_connect (b, "color-set", G_CALLBACK(color_set_cb), &prefs);
    return b;
  }
}

namespace
{
  struct HeaderColInfo
  {
    GtkTreeView * view;
    GtkTreeSelection * sel;
    GtkListStore * store;
    Prefs * prefs;
  };

  std::string get_header_column_string (GtkTreeModel * model)
  {
    std::string s;
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter_first (model, &iter)) do {
      gboolean enabled;
      char * key (0);
      gtk_tree_model_get (model, &iter, 0, &enabled, 1, &key, -1);
      if (enabled)
        s += std::string(key) + ",";
      g_free (key);
    } while (gtk_tree_model_iter_next (model, &iter));
    if (!s.empty())
      s.resize (s.size()-1); // strip trailing comma
    return s;
  }

  void header_column_up_cb (GtkButton *, gpointer user_data)
  {
    HeaderColInfo& info (*static_cast<HeaderColInfo*>(user_data));
    GtkTreeIter sel_iter;
    gtk_tree_selection_get_selected (info.sel, 0, &sel_iter);
    GtkTreePath * path = gtk_tree_model_get_path (GTK_TREE_MODEL(info.store), &sel_iter);
    if (gtk_tree_path_prev (path)) {
      GtkTreeIter prev_iter;
      gtk_tree_model_get_iter (GTK_TREE_MODEL(info.store), &prev_iter, path);
      gtk_list_store_move_after (info.store, &prev_iter, &sel_iter);
      info.prefs->set_string ("header-pane-columns", get_header_column_string (GTK_TREE_MODEL(info.store)));
    }
    gtk_tree_path_free (path);
  }

  void header_column_down_cb (GtkButton *, gpointer user_data)
  {
    HeaderColInfo& info (*static_cast<HeaderColInfo*>(user_data));
    GtkTreeIter sel_iter;
    gtk_tree_selection_get_selected (info.sel, 0, &sel_iter);
    GtkTreeIter next_iter = sel_iter;
    if (gtk_tree_model_iter_next (GTK_TREE_MODEL(info.store), &next_iter)) {
      gtk_list_store_move_after (info.store, &sel_iter, &next_iter);
      info.prefs->set_string ("header-pane-columns", get_header_column_string (GTK_TREE_MODEL(info.store)));
    }
  }

  void header_col_enabled_toggled_cb (GtkCellRendererToggle * ,
	                              gchar                 * path_str,
	                              gpointer                user_data)
  {
    HeaderColInfo& info (*static_cast<HeaderColInfo*>(user_data));
    GtkTreeModel * model = GTK_TREE_MODEL(info.store);
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string (path_str);

    // toggle the value
    gboolean fixed;
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get (model, &iter, 0, &fixed, -1);
    fixed = !fixed;
    gtk_list_store_set (info.store, &iter, 0, fixed, -1);
    info.prefs->set_string ("header-pane-columns", get_header_column_string (GTK_TREE_MODEL(info.store)));

    // clean up
    gtk_tree_path_free (path);
  }

  GtkWidget* header_columns_layout_new (Prefs& prefs)
  {
    typedef std::map<std::string,std::string> key_to_name_t;
    key_to_name_t key_to_name;
    key_to_name["action"] = _("Action");
    key_to_name["author"] = _("Author");
    key_to_name["bytes"] = _("Bytes");
    key_to_name["date"] = _("Date");
    key_to_name["lines"] = _("Lines");
    key_to_name["score"] = _("Score");
    key_to_name["state"] = _("State");
    key_to_name["subject"] = _("Subject");

    GtkListStore * store = gtk_list_store_new (3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING);
    const std::string column_keys = prefs.get_string ("header-pane-columns", "state,action,subject,score,author,lines,date");
    StringView v(column_keys), tok;
    while (v.pop_token (tok, ',')) {
      const std::string key (tok.to_string());
      GtkTreeIter iter;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, true,
                                        1, key.c_str(),
                                        2, key_to_name[key].c_str(), -1);
      key_to_name.erase (key);
    }
    foreach_const (key_to_name_t, key_to_name, it) {
      GtkTreeIter iter;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, false,
                                        1, it->first.c_str(),
                                        2, it->second.c_str(), -1);
    }

    GtkWidget * view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(store));
    GtkTreeSelection * sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(view));

    HeaderColInfo * info = g_new (HeaderColInfo, 1);
    info->store = store;
    info->prefs = &prefs;
    info->view = GTK_TREE_VIEW(view);
    info->sel = sel;
    g_object_weak_ref (G_OBJECT(view), (GWeakNotify)g_free, info);

    GtkCellRenderer * r = GTK_CELL_RENDERER (g_object_new (GTK_TYPE_CELL_RENDERER_TOGGLE, NULL));
    GtkTreeViewColumn * col = gtk_tree_view_column_new_with_attributes (_("Enabled"), r, "active", 0, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
    g_signal_connect (r, "toggled", G_CALLBACK(header_col_enabled_toggled_cb), info);
    r = GTK_CELL_RENDERER (g_object_new (GTK_TYPE_CELL_RENDERER_TEXT, NULL));
    col = gtk_tree_view_column_new_with_attributes (_("Column Name"), r, "text", 2, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
    gtk_tree_selection_set_mode (sel, GTK_SELECTION_BROWSE);
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first (GTK_TREE_MODEL(store), &iter);
    gtk_tree_selection_select_iter (sel, &iter);

    GtkWidget * f = gtk_frame_new (NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(f), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER(f), view);
    GtkWidget * hbox = gtk_hbox_new (false, PAD);
    gtk_box_pack_start (GTK_BOX(hbox), f, true, true, 0);
    GtkWidget * vbox = gtk_vbox_new (false, PAD);
    GtkWidget * up = gtk_button_new_from_stock (GTK_STOCK_GO_UP);
    gtk_box_pack_start (GTK_BOX(vbox), up, false, false, 0);
    GtkWidget * down = gtk_button_new_from_stock (GTK_STOCK_GO_DOWN);
    gtk_box_pack_start (GTK_BOX(vbox), down, false, false, 0);
    gtk_box_pack_start (GTK_BOX(hbox), vbox, false, false, 0);
    g_signal_connect (up, "clicked", G_CALLBACK(header_column_up_cb), info);
    g_signal_connect (down, "clicked", G_CALLBACK(header_column_down_cb), info);

    return hbox;
  }

  void font_toggled_cb (GtkToggleButton * tb, gpointer user_data)
  {
    const bool active (gtk_toggle_button_get_active (tb));
    gtk_widget_set_sensitive (GTK_WIDGET(user_data), active);
  }
}

PrefsDialog :: PrefsDialog (Prefs& prefs, GtkWindow* parent):
  _prefs (prefs)
{
  GtkWidget * dialog = gtk_dialog_new_with_buttons (_("Pan: Preferences"), parent,
                                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                                    NULL);
  gtk_window_set_role (GTK_WINDOW(dialog), "pan-preferences-dialog");
  g_signal_connect (dialog, "response", G_CALLBACK(response_cb), this);
  g_signal_connect_swapped (dialog, "destroy", G_CALLBACK(delete_prefs_dialog), this);
  GtkWidget * notebook = gtk_notebook_new ();

  int row (0);
  GtkWidget *h, *w, *l, *b, *t;
  t = HIG :: workarea_create ();
  HIG::workarea_add_section_title (t, &row, _("Mouse"));
    HIG :: workarea_add_section_spacer (t, row, 2);
    w = new_check_button (_("Single-click activates, rather than selects, _groups"), "single-click-activates-group", true, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("Single-click activates, rather than selects, _articles"), "single-click-activates-article", true, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
  HIG::workarea_add_section_divider (t, &row);
  HIG::workarea_add_section_title (t, &row, _("Groups"));
    HIG::workarea_add_section_spacer (t, row, 5);
    w = new_check_button (_("Get new headers in subscribed groups on _startup"), "get-new-headers-on-startup", false, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("Get new headers when _entering group"), "get-new-headers-when-entering-group", true, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("Mark entire group _read when leaving group"), "mark-group-read-when-leaving-group", false, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("Mark entire group read before getting _new headers"), "mark-group-read-before-xover", false, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("E_xpand all threads when entering group"), "expand-threads-when-entering-group", false, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
  HIG::workarea_add_section_divider (t, &row);
  HIG :: workarea_add_section_title (t, &row, _("Articles"));
    HIG :: workarea_add_section_spacer (t, row, 4);
    w = new_check_button (_("Space selects next article rather than next unread"), "space-selects-next-article", true, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("Smooth scrolling"), "smooth-scrolling", true, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_check_button (_("Clear article cache on shutdown"), "clear-article-cache-on-shutdown", false, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_spin_button ("newsrc-autosave-timeout-min", 0, 60, prefs);
    l = gtk_label_new(_("Minutes to autosave newsrc files."));
    HIG::workarea_add_row (t, &row, l, w);
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("_Behavior")));

  row = 0;
  t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Pane Layout"));
    std::string cur = _prefs.get_string ("pane-layout", "stacked-right");
    HIG :: workarea_add_section_spacer (t, row, 1);
    h = gtk_hbox_new (false, PAD);
    w = new_layout_radio (0, icon_layout_1, "stacked-top", cur, prefs);
    gtk_box_pack_start (GTK_BOX(h), w, false, false, 0);
    w = new_layout_radio (w, icon_layout_2, "stacked-bottom", cur, prefs);
    gtk_box_pack_start (GTK_BOX(h), w, false, false, 0);
    w = new_layout_radio (w, icon_layout_3, "stacked-left", cur, prefs);
    gtk_box_pack_start (GTK_BOX(h), w, false, false, 0);
    w = new_layout_radio (w, icon_layout_4, "stacked-right", cur, prefs);
    gtk_box_pack_start (GTK_BOX(h), w, false, false, 0);
    w = new_layout_radio (w, icon_layout_5, "stacked-vertical", cur, prefs);
    gtk_box_pack_start (GTK_BOX(h), w, false, false, 0);
    HIG::workarea_add_wide_control (t, &row, h);
  HIG :: workarea_add_section_divider (t, &row);
  HIG :: workarea_add_section_title (t, &row, _("Tasks"));
    HIG :: workarea_add_section_spacer (t, row, 6);
    cur = _prefs.get_string ("pane-orient", "groups,headers,body");
    w = new_orient_radio (0, _("1=Groups, 2=Headers, 3=Body"), "groups,headers,body", cur, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_orient_radio (w, _("1=Groups, 2=Body, 3=Headers"), "groups,body,headers", cur, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_orient_radio (w, _("1=Headers, 2=Groups, 3=Body"), "headers,groups,body", cur, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_orient_radio (w, _("1=Headers, 2=Body, 3=Groups"), "headers,body,groups", cur, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_orient_radio (w, _("1=Body, 2=Groups, 3=Headers"), "body,groups,headers", cur, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
    w = new_orient_radio (w, _("1=Body, 2=Headers, 3=Groups"), "body,headers,groups", cur, prefs);
    HIG :: workarea_add_wide_control (t, &row, w);
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("_Layout")));

  row = 0;
  t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Header Pane Columns"));
    HIG :: workarea_add_section_spacer(t, row, 1);
    HIG :: workarea_add_wide_control (t, &row, header_columns_layout_new (prefs));
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("_Headers")));

  row = 0;
  t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Fonts"));
    HIG :: workarea_add_section_spacer (t, row, 4);
    l = new_check_button (_("Use custom font in Group Pane:"), "group-pane-font-enabled", false, prefs);
    b = new_font_button ("group-pane-font", "Sans 10", prefs);
    g_signal_connect (l, "toggled", G_CALLBACK(font_toggled_cb), b);
    font_toggled_cb (GTK_TOGGLE_BUTTON(l), b);
    HIG :: workarea_add_row (t, &row, l, b);
    l = new_check_button (_("Use custom font in Header Pane:"), "header-pane-font-enabled", false, prefs);
    b = new_font_button ("header-pane-font", "Sans 10", prefs);
    g_signal_connect (l, "toggled", G_CALLBACK(font_toggled_cb), b);
    font_toggled_cb (GTK_TOGGLE_BUTTON(l), b);
    HIG :: workarea_add_row (t, &row, l, b);
    l = new_check_button (_("Use custom font in Body Pane:"), "body-pane-font-enabled", false, prefs);
    b = new_font_button ("body-pane-font", "Sans 10", prefs);
    g_signal_connect (l, "toggled", G_CALLBACK(font_toggled_cb), b);
    font_toggled_cb (GTK_TOGGLE_BUTTON(l), b);
    HIG :: workarea_add_row (t, &row, l, b);
    l = gtk_label_new_with_mnemonic (_("Monospace font:"));
    b = new_font_button ("monospace-font", "Monospace 10", prefs);
    HIG :: workarea_add_row (t, &row, l, b);
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("_Fonts")));

  row = 0;
  t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Header Pane"));
    HIG :: workarea_add_section_spacer(t, row, 5);
    h = gtk_hbox_new (false, PAD);
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Text:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-watched-fg", "black", prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Background:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-watched-bg", TANGO_CHAMELEON_LIGHT, prefs));
    HIG :: workarea_add_row (t, &row, _("Scores of 9999 or more:"), h);
    h = gtk_hbox_new (false, PAD);
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Text:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-high-fg", "black", prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Background:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-high-bg", TANGO_BUTTER_LIGHT, prefs));
    HIG :: workarea_add_row (t, &row, _("Scores from 5000 to 9998:"), h);
    h = gtk_hbox_new (false, PAD);
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Text:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-medium-fg", "black", prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Background:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-medium-bg", TANGO_SKY_BLUE_LIGHT, prefs));
    HIG :: workarea_add_row (t, &row, _("Scores from 1 to 4999:"), h);
    h = gtk_hbox_new (false, PAD);
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Text:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-low-fg", TANGO_ALUMINUM_2, prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Background:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-low-bg", "black", prefs));
    HIG :: workarea_add_row (t, &row, _("Scores from -9998 to -1:"), h);
    h = gtk_hbox_new (false, PAD);
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Text:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-ignored-fg", TANGO_ALUMINUM_4, prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), gtk_label_new (_("Background:")));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("score-color-ignored-bg", "black", prefs));
    HIG :: workarea_add_row (t, &row, _("Scores of -9999 or less:"), h);
  HIG :: workarea_add_section_divider (t, &row);
  HIG :: workarea_add_section_title (t, &row, _("Body Pane"));
    HIG :: workarea_add_section_spacer (t, row, 3);
    h = gtk_hbox_new (false, PAD_SMALL);
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("body-pane-color-quote-1", TANGO_CHAMELEON_DARK, prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("body-pane-color-quote-2", TANGO_ORANGE_DARK, prefs));
    pan_box_pack_start_defaults (GTK_BOX(h), new_color_button ("body-pane-color-quote-3", TANGO_PLUM_DARK, prefs));
    HIG :: workarea_add_row (t, &row, _("Quoted Text:"), h);
    HIG :: workarea_add_row (t, &row, _("URL:"), new_color_button ("body-pane-color-url", TANGO_SKY_BLUE_DARK, prefs));
    HIG :: workarea_add_row (t, &row, _("Signature:"), new_color_button ("body-pane-color-signature", TANGO_SKY_BLUE_LIGHT, prefs));
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("_Colors")));

  row = 0;
  t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Preferred Applications"));
    HIG :: workarea_add_section_spacer (t, row, 3);
    w = url_handler_new (prefs, "browser-mode", URL::get_environment(),
                                "custom-browser", "firefox", b);
    HIG :: workarea_add_row (t, &row, _("_Web Browser:"), w);
    w = url_handler_new (prefs, "mailer-mode", URL::get_environment(),
                                "custom-mailer", "thunderbird", b);
    HIG :: workarea_add_row (t, &row, _("_Mail Reader:"), w);
    w = editor_new (prefs);
    HIG :: workarea_add_row (t, &row, _("_Text Editor:"), w);
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("A_pplications")));

  row = 0;
  t = HIG :: workarea_create ();
  HIG :: workarea_add_section_title (t, &row, _("Upload Queue Options"));
  HIG :: workarea_add_section_spacer (t, row, 4);
  w = new_check_button (_("Always save article _information from Uploads to a file"), "upload-queue-save-enabled", false, prefs);
  HIG :: workarea_add_wide_control (t, &row, w);
  HIG :: workarea_add_section_title (t, &row, _("Upload Subject Line Appearance"));
  w = new_check_button (_("Append [_partof/total parts] style counter"), "upload-queue-append-partcounter-enabled", false, prefs);
  HIG :: workarea_add_wide_control (t, &row, w);
  w = new_check_button (_("Append su_bject for all posts"), "upload-queue-append-subject-enabled", false, prefs);
  HIG :: workarea_add_wide_control (t, &row, w);
  HIG :: workarea_finish (t, &row);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), t, gtk_label_new_with_mnemonic(_("_Upload options")));

  gtk_widget_show_all (notebook);
  gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area( GTK_DIALOG(dialog))), notebook, true, true, 0);
  _root = dialog;
}
