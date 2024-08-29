#include "format_list.h"

#define CORNER_FLAG "+"
#define ROW_LINE "-"
#define COL_PADDING " "
#define COL_LINE "|"

/**
 * 计算在控制台中输出的字符串长度
 * str 字符串
 **/
static int str_len(char *str){
    int len = 0;
    int offset = 2;
    int word_len = strlen(str);
    int i = 0;

    for(i = 0; i < word_len; i++){
        if(str[i] < 128){
            len += 1;
        } else {
            len += 2;
            i += offset;
        }
    }
    return len;
}
/**
 * 生成表格行边
 * col_width 表格的列宽
 * col_num 表格列数
 * edge 保存行边字符串
 **/
static void make_edge(const unsigned int *col_width, const unsigned int col_num, char *edge){
    strcat(edge, CORNER_FLAG);
    int col_line_counter;
    int row_line_counter;
    for( col_line_counter = 0; col_line_counter < col_num; col_line_counter++ ){
        strcat(edge, ROW_LINE);
        for( row_line_counter = 0; row_line_counter < col_width[col_line_counter]; row_line_counter++ ){
            strcat(edge, ROW_LINE);
        }
        strcat(edge, ROW_LINE);
        strcat(edge, CORNER_FLAG);
    }
    strcat(edge, "\r\n");
}

/**
 * 根据表格一列的宽度值，求出该列中最大的宽度
 * col_width 指向一列所有宽度值数组的指针
 * row_num 表格行数
 * return 最大宽度值
 **/
static int get_max_col_width(int *col_width, const unsigned int row_num){
    int max_col_width = 0;
    int i = 0;
    for(i = 0; i < row_num; i++){
        if( max_col_width < col_width[i]){
            max_col_width = col_width[i];
        }
    }
    return max_col_width;
}

/**
 * 根据表格列的宽度，求出每列最大的宽度
 * col_width 指向表格所有列宽度二维数组的指针
 * row_num 表格行数
 * col_num 表格列数
 * max_col_width 指向format_list结构体中的col_max_width的指针，用于保存表格每列最大的宽度
 **/
static void get_max_col(int **col_width, const unsigned int row_num, const unsigned int col_num, unsigned int *max_col_width){
    int i;
    int j;
    for(i = 0; i < col_num; i++){
        int max_width[ row_num ];
        for(j = 0; j < row_num; j++){
            max_width[j] = *((int *)col_width + (j * col_num) + i);
        }
        max_col_width[i] = get_max_col_width(max_width, row_num);
    }
}

/**
 * 得到表格中所有列的宽度值
 * t 指向表格结构体的指针
 * col_width 指向表格所有列宽度二维数组的指针，用于保存所有宽度值
 **/
static void get_col_width(format_list*t, int **col_width){
    int i = 0;
    int j;
    for(i = 0; i < t->row_num; i++){
        for(j = 0; j < t->col_num; j++){
            *((int *)col_width + i * t->col_num + j) = str_len(t->content[i][j]);
        }
    }
}

/**
 * 设置format_list结构体中col_max_width
 * t 指向表格结构体的指针
 **/
static void set_list_col_max_width(format_list*t){
    int col_width[t->row_num][t->col_num];
    get_col_width(t, (int **)col_width);
    get_max_col((int **)col_width, t->row_num, t->col_num, t->col_max_width);
}

/**
 * 得到表格的总宽
 * t 指向表格结构体的指针
 **/
static int get_list_width(format_list*t){
    int col_padding_width = 2 * t->col_num;
    int col_line_width = t->col_num + 1;
    int col_width = 0;
    int i;
    for(i = 0; i < t->col_num; i++){
        col_width += t->col_max_width[i];
    }
    return col_line_width + col_padding_width + col_width;
}

/**
 * 设置格式化表格中的空格
 * str 保存的字符串
 * space_num 空格的数量
 **/
static void set_space_txt(char *str, const unsigned int space_num){
    int i;
    for(i = 0; i < space_num; i++){
        strcat(str, " ");
    }
}

/**
 * 生成表格行边
 * row_id 行id
 * t 指向表格结构体的指针
 * row_txt 格式化输出的表格行边字符串
 **/
static void make_row(const unsigned int row_id, format_list*t, char *row_txt){
    int i;
    strcat(row_txt, COL_LINE);
    for(i = 0; i < t->col_num; i++){
        strcat(row_txt, COL_PADDING);
        int word_len = str_len(t->content[row_id][i]);
        strcat(row_txt, t->content[row_id][i]);
        set_space_txt(row_txt, t->col_max_width[i] - word_len);
        strcat(row_txt, COL_PADDING);
        strcat(row_txt, COL_LINE);
    }
}

#define FORMAT_SAFE_ADD_LENGTH (128)
static int format_list_get_string_length(format_list*t){
    int length = 0;
    set_list_col_max_width(t);
    int col_width = get_list_width(t);
    char row_txt[col_width + t->col_num];
    int i;

    memset(row_txt, 0, sizeof(row_txt));
    make_edge(t->col_max_width, t->col_num, row_txt);
    length += strlen(row_txt);
    /* strcat(formated_out, row_txt); */

    for( i = 0; i < t->row_num; i++){
        memset(row_txt, 0x0, sizeof(row_txt));
        make_row(i, t, row_txt);
        /* strcat(formated_out, row_txt); */
        /* strcat(formated_out, "\n"); */
        length += strlen(row_txt);
        length += strlen("\r\n");
        memset(row_txt, 0x0, sizeof(row_txt));
        make_edge(t->col_max_width, t->col_num, row_txt);
        /* strcat(formated_out, row_txt); */
        length += strlen(row_txt);
    }

    return length + FORMAT_SAFE_ADD_LENGTH;
}


/**
 * 格式化表格方法
 * t 指向表格结构体的指针
 * formated_out 格式化输出字符串
 **/
static char *format_format_list(format_list*t, int data_len){
    char *formated_out = NULL;
    set_list_col_max_width(t);
    int col_width = get_list_width(t);
    char row_txt[col_width + t->col_num];
    int i;

    if (t->strings) free(t->strings);

    t->strings = (char *) malloc (data_len);
    if (NULL == t->strings) return NULL;

    memset(t->strings, 0x0, data_len);

    formated_out = t->strings;

    memset(row_txt, 0x0, sizeof(row_txt));
    make_edge(t->col_max_width, t->col_num, row_txt);
    strcat(formated_out, row_txt);

    for( i = 0; i < t->row_num; i++) {
        memset(row_txt, 0x0, sizeof(row_txt));
        make_row(i, t, row_txt);
        strcat(formated_out, row_txt);
        strcat(formated_out, "\r\n");
        memset(row_txt, 0x0, sizeof(row_txt));
        make_edge(t->col_max_width, t->col_num, row_txt);
        strcat(formated_out, row_txt);
    }

    return formated_out;
}

char *format_list_run(format_list*t) {
    int length = format_list_get_string_length(t);
    if (length) {
        return format_format_list(t, length);
    }

    return NULL;
}

int format_list_init(format_list*t, int row, int col) {
    int index = 0;

    memset(t, 0x0, sizeof(*t));

    t->row_num   = row;
    t->col_num   = col;

    t->col_max_width     = (unsigned int *) malloc (sizeof(int) * t->col_num);
    if (NULL == t->col_max_width) {
        printf ("[%s:%s:%d]malloc col_max_width instance failure\n", 
                __FILE__, __func__, __LINE__);
        goto ERR0;
    }

    memset(t->col_max_width, 0x0, sizeof(int) * t->col_num);

    t->content = (char ***)malloc(sizeof(char **) * t->row_num);
    if (NULL == t->content) {
        printf ("[%s:%s:%d]malloc content instance failure\n", 
                __FILE__, __func__, __LINE__);
        goto ERR1;
    }

    memset(t->content, 0x0, sizeof(char **) * t->row_num);

    for (index = 0; index < t->row_num; index++) {
        t->content[index] = (char **)malloc(sizeof(char *) * t->col_num);
        if (NULL == t->content[index]) {
            printf ("[%s:%s:%d]malloc content[%d] instance failure\n", 
                    __FILE__, __func__, __LINE__, index);
            goto ERR2;
        }

        memset(t->content[index], 0x0, sizeof(char *) * t->col_num);
    }

    return 0;
ERR2:
    for (index = 0; index < t->row_num; index++) {
        if (t->content[index]) free(t->content[index]);
    }
    if (t->content) free(t->content);
ERR1:
    if (t->col_max_width) free(t->col_max_width);
ERR0:
    return -1;
}

int format_list_set_data(format_list*t, int row, int col, const char *data) {
    int dataLen = strlen(data) + 1;
    t->content[row][col] = (char *) malloc (dataLen);
    if (NULL == t->content[row][col]) {
        printf ("[%s:%s:%d]malloc content[%d][%d] instance failure\n", 
                __FILE__, __func__, __LINE__, row, col);
        return -1;
    }

    strcpy(t->content[row][col], data);

    return 0;
}

int format_list_set_data_title(format_list*t, const char *data[]) {
    int index = 0; 

    for (index = 0; index < t->col_num; index++) {
        int dataLen = strlen(data[index]) + 1;
        t->content[0][index] = (char *) malloc (dataLen);
        if (NULL == t->content[0][index]) {
            printf ("[%s:%s:%d]malloc content[%d][%d] instance failure\n", 
                    __FILE__, __func__, __LINE__, 0, index);
            return -1;
        }

        strcpy(t->content[0][index], data[index]);
    }

    return 0;
}

int format_list_set_data_int_16(format_list*t, int row, int col, int value) {
    char stringg[64];

    snprintf (stringg, sizeof(stringg) - 1, "%x", value);

    format_list_set_data(t, row, col, stringg);

    return 0;
}

int format_list_set_data_int(format_list*t, int row, int col, int value) {
    char stringg[64];

    snprintf (stringg, sizeof(stringg) - 1, "%d", value);

    format_list_set_data(t, row, col, stringg);

    return 0;
}

int format_list_set_data_float(format_list*t, int row, int col, float value) {
    char stringg[64];

    snprintf (stringg, sizeof(stringg) - 1, "%f", value);

    format_list_set_data(t, row, col, stringg);

    return 0;
}

void format_list_exit(format_list*t) {
    int row = 0;
    int col = 0;

    for (row = 0; row < t->row_num; row++) {
        for (col = 0; col < t->col_num; col++) {
            if (t->content[row][col]) {
                free(t->content[row][col]);
                t->content[row][col] = NULL;
            }
        }

        if (t->content[row]) {
            free(t->content[row]);
            t->content[row] = NULL;
        }
    }

    if (t->content) {
        free(t->content);
        t->content = NULL;
    }

    if (t->col_max_width) {
        free(t->col_max_width);
        t->col_max_width = NULL;
    }

    if (t->strings) {
        free(t->strings);
        t->strings = NULL;
    }
}


#ifdef TABLE_TEST 
int main(void){

    format_list t;

    format_list_init(&t, 8, 3);
    const char *title[3] = {
        "name",
        "class",
        "core"
    };
    format_list_set_data_title(&t, title); /*row 0*/

    format_list_set_data(&t, 1, 0, "AAAAAAAAAAAAAAAA"); /*row 1*/
    format_list_set_data(&t, 1, 1, "test");
    format_list_set_data(&t, 1, 2, "BB");

    format_list_set_data(&t, 2, 0, "2"); /*row 2*/
    format_list_set_data(&t, 2, 1, "CCCCCCCCCCCCC");
    format_list_set_data(&t, 2, 2, "测试");

    format_list_set_data(&t, 3, 0, "3");/*row 3*/
    format_list_set_data(&t, 3, 1, "EEEEEEEEEEEEEE");
    format_list_set_data(&t, 3, 2, "FFFFFFFFFFFFFFFFF");

    format_list_set_data(&t, 4, 0, "GGGGGGGGGGGG");/*row 4*/
    format_list_set_data(&t, 4, 1, "HHHHHHHHHHHH");
    format_list_set_data(&t, 4, 2, "IIIIIIII");

    format_list_set_data(&t, 5, 0, "DDDDDD");/*row 5*/
    format_list_set_data(&t, 5, 1, "HHHHHHHHH");
    format_list_set_data(&t, 5, 2, "UUUUUUUUU");

    format_list_set_data(&t, 6, 0, "UUUUUUUUU");/*row 6*/
    format_list_set_data_int(&t, 6, 1, 198);
    format_list_set_data_int(&t, 6, 2, 199);

    format_list_set_data(&t, 7, 0, "JJJJJJJ");/*row 7*/
    format_list_set_data_float(&t, 7, 1, 198.8);
    format_list_set_data_float(&t, 7, 2, 199.9);

    char *format = format_list_run(&t);
    if (NULL != format) {
        printf("%s", format);
    }

    format_list_exit(&t);


    return 0;
}
#endif
