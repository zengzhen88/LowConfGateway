#ifndef __SVN_TABLE_C_H__
#define __SVN_TABLE_C_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct FORMAT_LIST {
  unsigned int row_num;
  unsigned int col_num;
  unsigned int *col_max_width;
  char ***content;
  char *strings;
} format_list;

extern char *format_list_run(format_list*t);
extern int format_list_init(format_list*t, int row, int col);
extern int format_list_set_data(format_list*t, int row, int col, const char *data);
extern int format_list_set_data_title(format_list*t, const char *data[]);
extern int format_list_set_data_float(format_list*t, int row, int col, float value);
extern int format_list_set_data_int(format_list*t, int row, int col, int value);
extern int format_list_set_data_int_16(format_list*t, int row, int col, int value);
extern void format_list_exit(format_list*t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__SVN_TABLE_C_H__*/
