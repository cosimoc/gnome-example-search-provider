#include <config.h>

#include <gio/gio.h>
#include <string.h>
#include <glib/gi18n.h>
#include <gdk/gdkx.h>

#include "search-example-provider-generated.h"

#define SEARCH_PROVIDER_INACTIVITY_TIMEOUT 12000 /* milliseconds */

typedef GApplicationClass SearchExampleProviderAppClass;
typedef struct _SearchExampleProviderApp SearchExampleProviderApp;

struct _SearchExampleProviderApp {
  GApplication parent;

  guint name_owner_id;
  GDBusObjectManagerServer *object_manager;
  SearchExampleShellSearchProvider2 *skeleton;
};

GType search_example_provider_app_get_type (void);

#define SEARCH_TYPE_EXAMPLE_PROVIDER_APP search_example_provider_app_get_type()
#define SEARCH_EXAMPLE_PROVIDER_APP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SEARCH_TYPE_EXAMPLE_PROVIDER_APP, SearchExampleProviderApp))

G_DEFINE_TYPE (SearchExampleProviderApp, search_example_provider_app, G_TYPE_APPLICATION)

static void
handle_get_initial_result_set (SearchExampleShellSearchProvider2  *skeleton,
                               GDBusMethodInvocation              *invocation,
                               gchar                             **terms,
                               gpointer                            user_data)
{
  gchar *joined_terms = g_strjoinv (" ", terms);

  g_print ("****** GetInitialResultSet() called with %s\n", joined_terms);
  g_free (joined_terms);

  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(as)", NULL));
}

static void
handle_get_subsearch_result_set (SearchExampleShellSearchProvider2  *skeleton,
                                 GDBusMethodInvocation              *invocation,
                                 gchar                             **previous_results,
                                 gchar                             **terms,
                                 gpointer                            user_data)
{
  gchar *joined_terms = g_strjoinv (" ", terms);

  g_print ("****** GetSubSearchResultSet() called with %s\n", joined_terms);
  g_free (joined_terms);

  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(as)", NULL));
}

static void
handle_get_result_metas (SearchExampleShellSearchProvider2  *skeleton,
                         GDBusMethodInvocation              *invocation,
                         gchar                             **results,
                         gpointer                            user_data)
{
  gint idx;

  g_print ("****** GetResultMetas() called for results \n");

  for (idx = 0; results[idx] != NULL; idx++)
    g_print ("   %s\n", results[idx]);

  g_print ("\n");

  g_dbus_method_invocation_return_value (invocation,
                                         g_variant_new ("(aa{sv})", NULL));
}

static void
handle_launch_search (SearchExampleShellSearchProvider2  *skeleton,
                      GDBusMethodInvocation              *invocation,
                      gchar                             **terms,
                      guint32                             timestamp,
                      gpointer                            user_data)
{
  gchar *joined_terms = g_strjoinv (" ", terms);

  g_print ("****** LaunchSearch() called with %s\n", joined_terms);
  g_free (joined_terms);

  g_dbus_method_invocation_return_value (invocation, NULL);
}

static void
handle_activate_result (SearchExampleShellSearchProvider2  *skeleton,
                        GDBusMethodInvocation              *invocation,
                        gchar                              *result,
                        gchar                             **terms,
                        guint32                             timestamp,
                        gpointer                            user_data)
{
  gchar *joined_terms = g_strjoinv (" ", terms);

  g_print ("****** ActivateResult() called for %s and result %s\n",
           joined_terms, result);
  g_free (joined_terms);

  g_dbus_method_invocation_return_value (invocation, NULL);
}

static void
search_provider_name_acquired_cb (GDBusConnection *connection,
                                  const gchar     *name,
                                  gpointer         user_data)
{
  g_print ("Search provider name acquired: %s\n", name);
}

static void
search_provider_name_lost_cb (GDBusConnection *connection,
                              const gchar     *name,
                              gpointer         user_data)
{
  g_print ("Search provider name lost: %s\n", name);
}

static void
search_provider_bus_acquired_cb (GDBusConnection *connection,
                                 const gchar *name,
                                 gpointer user_data)
{
  SearchExampleProviderApp *self = user_data;

  self->object_manager = g_dbus_object_manager_server_new ("/org/gnome/SearchExample/SearchProvider");
  self->skeleton = search_example_shell_search_provider2_skeleton_new ();

  g_signal_connect (self->skeleton, "handle-get-initial-result-set",
                    G_CALLBACK (handle_get_initial_result_set), self);
  g_signal_connect (self->skeleton, "handle-get-subsearch-result-set",
                    G_CALLBACK (handle_get_subsearch_result_set), self);
  g_signal_connect (self->skeleton, "handle-get-result-metas",
                    G_CALLBACK (handle_get_result_metas), self);
  g_signal_connect (self->skeleton, "handle-activate-result",
                    G_CALLBACK (handle_activate_result), self);
  g_signal_connect (self->skeleton, "handle-launch-search",
                    G_CALLBACK (handle_launch_search), self);

  g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (self->skeleton),
                                    connection,
                                    "/org/gnome/SearchExample/SearchProvider", NULL);
  g_dbus_object_manager_server_set_connection (self->object_manager, connection);
}

static void
search_provider_app_dispose (GObject *obj)
{
  SearchExampleProviderApp *self = SEARCH_EXAMPLE_PROVIDER_APP (obj);

  if (self->name_owner_id != 0) {
    g_bus_unown_name (self->name_owner_id);
    self->name_owner_id = 0;
  }

  if (self->skeleton != NULL) {
    g_dbus_interface_skeleton_unexport (G_DBUS_INTERFACE_SKELETON (self->skeleton));
    g_clear_object (&self->skeleton);
  }

  g_clear_object (&self->object_manager);

  G_OBJECT_CLASS (search_example_provider_app_parent_class)->dispose (obj);
}

static void
search_provider_app_startup (GApplication *app)
{
  SearchExampleProviderApp *self = SEARCH_EXAMPLE_PROVIDER_APP (app);

  G_APPLICATION_CLASS (search_example_provider_app_parent_class)->startup (app);

  /* hold indefinitely if we're asked to persist */
  if (g_getenv ("EXAMPLE_SEARCH_PROVIDER_PERSIST") != NULL)
    g_application_hold (app);

  self->name_owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                                        "org.gnome.SearchExample.SearchProvider",
                                        G_BUS_NAME_OWNER_FLAGS_NONE,
                                        search_provider_bus_acquired_cb,
                                        search_provider_name_acquired_cb,
                                        search_provider_name_lost_cb,
                                        app, NULL);
}

static void
search_example_provider_app_init (SearchExampleProviderApp *self)
{
  GApplication *app = G_APPLICATION (self);

  g_application_set_inactivity_timeout (app, SEARCH_PROVIDER_INACTIVITY_TIMEOUT);
  g_application_set_application_id (app, "org.gnome.SearchExample.SearchProvider");
  g_application_set_flags (app, G_APPLICATION_IS_SERVICE);
}

static void
search_example_provider_app_class_init (SearchExampleProviderAppClass *klass)
{
  GApplicationClass *aclass = G_APPLICATION_CLASS (klass);
  GObjectClass *oclass = G_OBJECT_CLASS (klass);

  aclass->startup = search_provider_app_startup;
  oclass->dispose = search_provider_app_dispose;
}

static GApplication *
search_example_provider_app_new (void)
{
  g_type_init ();

  return g_object_new (search_example_provider_app_get_type (),
                       NULL);
}

int
main (int argc,
      char **argv)
{
  GApplication *app;
  gint res;

  gtk_init (&argc, &argv);

  app = search_example_provider_app_new ();
  res = g_application_run (app, argc, argv);
  g_object_unref (app);

  return res;
}
