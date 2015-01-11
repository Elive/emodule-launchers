#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_MOUNT

#include <e.h>
#include "e_mod_places.h"



#define FSTAB_FILE "/etc/fstab"
#define MTAB_FILE "/proc/mounts"

static Ecore_Timer *mtab_timer = NULL;
static Eina_List *know_mounts = NULL;


void
_places_mount_volume_add(const char *mpoint, const char *fstype)
{
   Volume *vol;

   // printf("PLACES: found mount: %s (%s)\n", mpoint, fstype);
   vol = places_volume_add(mpoint, EINA_TRUE);
   vol->valid = EINA_TRUE;
   vol->mounted = EINA_FALSE;
   eina_stringshare_replace(&vol->label,  ecore_file_file_get(mpoint));
   eina_stringshare_replace(&vol->mount_point, mpoint);
   eina_stringshare_replace(&vol->fstype, fstype);

   know_mounts = eina_list_append(know_mounts, vol);
}

char *
_places_mount_file_read(const char *path)
{
   char buf[10000]; //   :/
   FILE * f;
   size_t len;
   
   f = fopen(path, "r");
   if (!f)
      return NULL;

   len = fread(buf, sizeof(char), sizeof(buf), f);
   fclose(f);

   if (len < 1)
      return NULL;

   // printf("PLACES: readed %ld\n", len);
   buf[++len] = '\0';
   return strdup(buf);
}

void
_places_mount_mtab_parse(void)
{
   char *contents;
   char **lines;
   unsigned int i, num_splits;
   char mpoint[256];
   Eina_List *to_search, *l, *l2;
   Volume *vol;

   contents = _places_mount_file_read("/proc/mounts");
   if (!contents)
      return;

   lines = eina_str_split(contents, "\n", 0);
   if (!lines)
      return;

   to_search = eina_list_clone(know_mounts);

   for (i = 0; lines[i]; i++)
   {
      num_splits = sscanf(lines[i], "%*s %s %*s %*s %*d %*d", mpoint);
      if (num_splits != 1)
         continue;

      EINA_LIST_FOREACH_SAFE(to_search, l, l2, vol)
      {
         if (strcmp(vol->mount_point, mpoint))
           continue;

         // printf("PLACES: Mounted: %s\n", mpoint);
         to_search = eina_list_remove(to_search, vol);
         if (vol->mounted == EINA_FALSE)
         {
            vol->mounted = EINA_TRUE;
            places_volume_update(vol);
         }
         break;
      }
      continue;
   }

   EINA_LIST_FREE(to_search, vol)
   {
      // printf("PLACES: NOT Mounted: %s\n", vol->mount_point);
      if (vol->mounted == EINA_TRUE)
      {
         vol->mounted = EINA_FALSE;
         places_volume_update(vol);
      }
   }

   free(lines[0]);
   free(lines);
   free(contents);
}

void
_places_mount_fstab_parse(void)
{
   char *contents, *line;
   char **lines;
   unsigned int i, num_splits;
   char mpoint[256];
   char fstype[64];

   contents = _places_mount_file_read(FSTAB_FILE);
   if (!contents)
      return;

   lines = eina_str_split(contents, "\n", 0);
   if (!lines)
      return;

   for (i = 0; lines[i]; i++)
   {
      line = lines[i];

      if (!line[0] || line[0] == '\n' || line[0] == '#')
         continue;

      num_splits = sscanf(line, "%*s %s %s %*s %*d %*d", mpoint, fstype);
      if (num_splits != 2)
         continue;

      if (!strcmp(fstype, "nfs") ||
          !strcmp(fstype, "cifs"))
      {
         _places_mount_volume_add(mpoint, fstype);
      }
   }

   free(lines[0]);
   free(lines);
   free(contents);
}

static Eina_Bool
_places_mount_mtab_timer_cb(void *data)
{
   _places_mount_mtab_parse();
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
places_mount_init(void)
{
   printf("PLACES: mount: init()\n");

   if (!ecore_file_exists(FSTAB_FILE) ||
       !ecore_file_exists(MTAB_FILE))
   {
      printf("PLACES: mount: Cannot find required files\n");
      return EINA_FALSE;
   }

   _places_mount_fstab_parse();
   _places_mount_mtab_parse();

   mtab_timer = ecore_timer_add(4.0, _places_mount_mtab_timer_cb, NULL);
   return EINA_TRUE;
}

void
places_mount_shutdown(void)
{
   printf("PLACES: mtab: shutdown()\n");
   if (mtab_timer) ecore_timer_del(mtab_timer);
   if (know_mounts) eina_list_free(know_mounts);
}


#endif
