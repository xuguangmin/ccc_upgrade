#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__

extern int  load_configure();
extern void close_configure();

extern int check_login(const char *login_info);
extern const char *get_upgrade_dir();

#endif  /* __CONFIGURE_H__ */
