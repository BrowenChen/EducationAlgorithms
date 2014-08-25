/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Container Class for Covariates
 * Copyright 2010, 2011 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:covariates
 * @title:OscatsCovariates
 * @short_description: Covariates Container Class
 */

#include "covariates.h"

G_DEFINE_TYPE(OscatsCovariates, oscats_covariates, G_TYPE_OBJECT);

static void oscats_covariates_finalize (GObject *object);
                   
static void oscats_covariates_class_init (OscatsCovariatesClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->finalize = oscats_covariates_finalize;

}

static gint compare(gconstpointer a, gconstpointer b)
{
  return b-a;
}

static void oscats_covariates_init (OscatsCovariates *self)
{
  self->names = g_tree_new(compare);
  self->data = g_array_new(FALSE, TRUE, sizeof(gdouble));
}

static void oscats_covariates_finalize (GObject *object)
{
  OscatsCovariates *self = OSCATS_COVARIATES(object);
  g_tree_destroy(self->names);
  g_array_free(self->data, TRUE);
  G_OBJECT_CLASS(oscats_covariates_parent_class)->finalize(object);
}

/**
 * oscats_covariates_from_string:
 * @name: the string name of the covariate
 *
 * A wrapper of g_quark_from_string() for language bindings.
 *
 * Returns: the covariate name as a #GQuark
 */
GQuark oscats_covariates_from_string(const gchar *name)
{
  return g_quark_from_string(name);
}

/**
 * oscats_covariates_as_string:
 * @name: a #GQuark covariate name
 *
 * A wrapper of g_quark_to_string() for language bindings.
 *
 * Returns: the string form of @characteristic
 */
const gchar * oscats_covariates_as_string(GQuark name)
{
  return g_quark_to_string(name);
}

/**
 * oscats_covariates_num:
 * @covariates: an #OscatsCovariates
 *
 * Returns: the number of covariates in @covariates
 */
guint oscats_covariates_num(const OscatsCovariates *covariates)
{
  g_return_val_if_fail(OSCATS_IS_COVARIATES(covariates), 0);
  return g_tree_nnodes(covariates->names);
}

typedef struct { GQuark *names; guint i; } namesCollector;
static gboolean collectNames(gpointer name, gpointer pos, gpointer data)
{
  namesCollector *collect = (namesCollector*)data;
  collect->names[collect->i++] = GPOINTER_TO_UINT(name);
  return FALSE;
}

/**
 * oscats_covariates_list:
 * @covariates: an #OscatsCovariates
 *
 * Returns: (transfer full): a list of the covariate #GQuark names
 */
GQuark *oscats_covariates_list(const OscatsCovariates *covariates)
{
  namesCollector collector = { NULL, 0 };
  guint num = oscats_covariates_num(covariates);
  if (num > 0)
  {
    collector.names = g_new(GQuark, num);
    g_tree_foreach(covariates->names, collectNames, &collector);
  }  
  return collector.names;
}

/**
 * oscats_covariates_set:
 * @covariates: an #OscatsCovariates object
 * @name: the covariate name (as a #GQuark)
 * @value: the covariate value
 *
 * Sets the covariate @name to @value.
 */
void oscats_covariates_set(OscatsCovariates *covariates,
                           GQuark name, gdouble value)
{
  gdouble *p;
  g_return_if_fail(OSCATS_IS_COVARIATES(covariates) && name != 0);
  p = g_tree_lookup(covariates->names, GUINT_TO_POINTER(name));
  if (p)
    *p = value;
  else
  {		// Add new covariate
    g_array_append_val(covariates->data, value);
    g_tree_insert(covariates->names, GUINT_TO_POINTER(name),
                  &g_array_index(covariates->data, gdouble,
                                 covariates->data->len));
  }
}

/**
 * oscats_covariates_set_by_name:
 * @covariates: an #OscatsCovariates object
 * @name: the covariate name
 * @value: the covariate value
 *
 * Sets the covariate @name to @value.  The #GQuark version
 * oscats_covariates_set() is faster.
 */
void oscats_covariates_set_by_name(OscatsCovariates *covariates,
                                   const gchar *name, gdouble value)
{
  oscats_covariates_set(covariates, g_quark_from_string(name), value);
}

/**
 * oscats_covariates_get:
 * @covariates: an #OscatsCovariates object
 * @name: the covariate name (as a #GQuark)
 *
 * Returns: the value of covariate @name, or 0.
 */
gdouble oscats_covariates_get(const OscatsCovariates *covariates, GQuark name)
{
  gdouble *p;
  g_return_val_if_fail(OSCATS_IS_COVARIATES(covariates) && name != 0, 0);
  p = g_tree_lookup(covariates->names, GUINT_TO_POINTER(name));
  return (p ? *p : 0);
}

/**
 * oscats_covariates_get_by_name:
 * @covariates: an #OscatsCovariates object
 * @name: the covariate name
 *
 * The #GQuark version oscats_covariates_get() is faster.
 *
 * Returns: the value of covariate @name, or 0.
 */
gdouble oscats_covariates_get_by_name(const OscatsCovariates *covariates,
                                      const gchar *name)
{
  return oscats_covariates_get(covariates, g_quark_from_string(name));
}
