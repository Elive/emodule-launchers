#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define D_(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define D_(string) (string)
#endif

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x0090
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

typedef struct _Config Config;
struct _Config 
{
   E_Module *module;
   E_Config_Dialog *cfd;

   Eina_List *conf_items;

   /* config file version */
   int version;

   const char *fm;
   Eina_Bool auto_mount;
   Eina_Bool boot_mount;
   Eina_Bool auto_open;
   Eina_Bool show_menu;
   Eina_Bool hide_header;
   Eina_Bool autoclose_popup;

   Eina_Bool show_home;
   Eina_Bool show_desk;
   Eina_Bool show_trash;
   Eina_Bool show_root;
   Eina_Bool show_temp;
   Eina_Bool show_bookm;
};

typedef struct _Config_Item Config_Item;
struct _Config_Item 
{
   const char *id;

   int switch2;
};

typedef struct _Instance Instance;
struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_main;
   Evas_Object *o_icon;
   E_Gadcon_Popup *popup;
   Eina_Bool horiz;
   E_Menu *menu;
   Config_Item *conf_item;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

E_Config_Dialog *e_int_config_places_module(E_Comp *comp, const char *params);
void places_menu_augmentation(void);
void places_popups_close(void);

extern Config *places_conf;
extern Eina_List *instances;

#endif
