#include <e.h>
#include "config.h"
#include "e_mod_main.h"
#include "e_mod_places.h"

struct _E_Config_Dialog_Data
{
   Eina_Bool auto_mount;
   Eina_Bool boot_mount;
   Eina_Bool auto_open;
   Eina_Bool fm_chk;
   Eina_Bool show_menu;
   Eina_Bool hide_header;
   Eina_Bool autoclose_popup;
   Eina_Bool show_home;
   Eina_Bool show_desk;
   Eina_Bool show_trash;
   Eina_Bool show_root;
   Eina_Bool show_temp;
   Eina_Bool show_bookm;
   Evas_Object *entry;
   Evas_Object *auto_open_chk;
};

/* Local Function Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

/* External Functions */
E_Config_Dialog *
e_int_config_places_module(Evas_Object *parent, const char *params)
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[PATH_MAX];

   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Places", "fileman/places")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-places.edj", places_conf->module->dir);

   /* create new config dialog */
   cfd = e_config_dialog_new(parent, D_("Places Settings"), "Places",
                             "fileman/places", buf, 0, v, NULL);
   places_conf->cfd = cfd;
   return cfd;
}

/* Local Functions */
static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   places_conf->cfd = NULL;
   E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   /* load a temp copy of the config variables */
   cfdata->auto_mount = places_conf->auto_mount;
   cfdata->boot_mount = places_conf->boot_mount;
   cfdata->auto_open = places_conf->auto_open;
   cfdata->show_menu = places_conf->show_menu;
   cfdata->hide_header = places_conf->hide_header;
   cfdata->autoclose_popup = places_conf->autoclose_popup;
   cfdata->show_home = places_conf->show_home;
   cfdata->show_desk = places_conf->show_desk;
   cfdata->show_trash = places_conf->show_trash;
   cfdata->show_root = places_conf->show_root;
   cfdata->show_temp = places_conf->show_temp;
   cfdata->show_bookm = places_conf->show_bookm;
}

static void
_mount_on_insert_chk_changed_cb(void *data, Evas_Object *obj, void *event __UNUSED__)
{
   E_Config_Dialog_Data *cfdata = data;

   if (elm_check_state_get(obj))
     elm_object_disabled_set(cfdata->auto_open_chk, EINA_FALSE);
   else
     {
        elm_object_disabled_set(cfdata->auto_open_chk, EINA_TRUE);
        elm_check_state_set(cfdata->auto_open_chk, EINA_FALSE);
     }
}

static void
_custom_fm_chk_changed_cb(void *data, Evas_Object *obj, void *event __UNUSED__)
{
   Evas_Object *en = data;

   elm_object_disabled_set(en, !elm_check_state_get(obj));
   if (!elm_check_state_get(obj))
     elm_object_text_set(en, NULL);
}

static void
_all_changed_cb(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   e_config_dialog_changed_set((E_Config_Dialog *)data, EINA_TRUE);
}

#define CHECKBOX(_text_, _pointer_)                                      \
   chk = elm_check_add(box2);                                            \
   elm_object_text_set(chk, _text_);                                     \
   elm_check_state_pointer_set(chk, _pointer_);                          \
   evas_object_smart_callback_add(chk, "changed", _all_changed_cb, cfd); \
   E_EXPAND(chk);                                                        \
   E_FILL(chk);                                                          \
   elm_box_pack_end(box2, chk);                                          \
   evas_object_show(chk);

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *win = cfd->dia->win;
   Evas_Object *box, *box2, *frame, *chk, *en;

   box = elm_box_add(win);
   E_EXPAND(box); E_FILL(box);
   evas_object_show(box);

   // general
   frame = elm_frame_add(box);
   elm_object_text_set(frame, D_("General"));
   E_EXPAND(frame); E_FILL(frame);
   elm_box_pack_end(box, frame);
   evas_object_show(frame);

   box2 = elm_box_add(win);
   E_EXPAND(box2); E_FILL(box2);
   elm_object_content_set(frame, box2);
   evas_object_show(box2);

   CHECKBOX(D_("Show in main menu"), &cfdata->show_menu);
   CHECKBOX(D_("Hide the gadget header"), &cfdata->hide_header);
   CHECKBOX(D_("Auto close the popup"), &cfdata->autoclose_popup);
   CHECKBOX(D_("Mount volumes at boot"), &cfdata->boot_mount);
   CHECKBOX(D_("Mount volumes on insert"), &cfdata->auto_mount);
   evas_object_smart_callback_add(chk, "changed", _mount_on_insert_chk_changed_cb, cfdata);
   CHECKBOX(D_("Open filemanager on insert"), &cfdata->auto_open);
   elm_object_disabled_set(chk, !cfdata->auto_mount);
   cfdata->auto_open_chk = chk;

   // custom fm chk + entry
   en = elm_entry_add(win);

   chk = elm_check_add(box2);
   elm_object_text_set(chk, D_("Use a custom file manager"));
   if (places_conf->fm && places_conf->fm[0] != '\0')
     elm_check_state_set(chk, EINA_TRUE);
   evas_object_smart_callback_add(chk, "changed", _custom_fm_chk_changed_cb, en);
   evas_object_smart_callback_add(chk, "changed", _all_changed_cb, cfd);
   E_EXPAND(chk); E_FILL(chk);
   elm_box_pack_end(box2, chk);
   evas_object_show(chk);

   elm_entry_scrollable_set(en, EINA_TRUE);
   elm_entry_single_line_set(en, EINA_TRUE);
   elm_object_text_set(en, places_conf->fm);
   elm_object_disabled_set(en, !elm_check_state_get(chk));
   evas_object_smart_callback_add(en, "changed,user", _all_changed_cb, cfd);
   E_EXPAND(en); E_FILL(en);
   elm_box_pack_end(box2, en);
   evas_object_show(en);
   cfdata->entry = en;

   // show in menu
   frame = elm_frame_add(box);
   elm_object_text_set(frame, D_("Show in menu"));
   E_EXPAND(frame); E_FILL(frame);
   elm_box_pack_end(box, frame);
   evas_object_show(frame);

   box2 = elm_box_add(win);
   E_EXPAND(box2); E_FILL(box2);
   elm_object_content_set(frame, box2);
   evas_object_show(box2);

   CHECKBOX(D_("Home"), &cfdata->show_home);
   CHECKBOX(D_("Desktop"), &cfdata->show_desk);
   CHECKBOX(D_("Trash"), &cfdata->show_trash);
   CHECKBOX(D_("Filesystem"), &cfdata->show_root);
   CHECKBOX(D_("Temp"), &cfdata->show_temp);
   CHECKBOX(D_("Favorites"), &cfdata->show_bookm);

   return box;
}

#undef CHECKBOX

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   places_conf->show_menu = cfdata->show_menu;
   places_conf->hide_header = cfdata->hide_header;
   places_conf->autoclose_popup = cfdata->autoclose_popup;
   places_conf->auto_mount = cfdata->auto_mount;
   places_conf->boot_mount = cfdata->boot_mount;
   places_conf->auto_open = cfdata->auto_open;
   places_conf->show_home = cfdata->show_home;
   places_conf->show_desk = cfdata->show_desk;
   places_conf->show_trash = cfdata->show_trash;
   places_conf->show_root = cfdata->show_root;
   places_conf->show_temp = cfdata->show_temp;
   places_conf->show_bookm = cfdata->show_bookm;

   eina_stringshare_del(places_conf->fm);
   places_conf->fm = eina_stringshare_add(elm_object_text_get(cfdata->entry));

   e_config_save_queue();
   places_update_all_gadgets();
   places_menu_augmentation();
   return 1;
}
