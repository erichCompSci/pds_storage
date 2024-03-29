/* basic_cmdline.h */

/* File autogenerated by gengetopt version 2.4  */

#ifndef _basic_cmdline_h
#define _basic_cmdline_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Don't define PACKAGE and VERSION if we use automake.  */
#if defined PACKAGE
#  undef PACKAGE
#endif
#define PACKAGE "pds-tests"
#if defined VERSION
#  undef VERSION
#endif
#define VERSION "0.1"

struct gengetopt_args_info {
  char * hostname_arg;	/* hostname to contact.  */
  int port_arg;	/* port number.  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int hostname_given ;	/* Whether hostname was given.  */
  int port_given ;	/* Whether port was given.  */
} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _basic_cmdline_h */
