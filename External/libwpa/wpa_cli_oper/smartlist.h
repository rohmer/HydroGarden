

#ifndef SMP_SMARTLIST_H
#define SMP_SMARTLIST_H

//¹¦ÄÜÒÑ¾­ÔÚwin7 ºÍ linux ÉÏ²âÊÔ¹ý

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <stdint.h>
#else
#include <sys/types.h>
#endif

#ifndef INLINE
    #ifdef _WIN32
        #define INLINE __inline
    #else
        #define INLINE inline
    #endif
#endif

#define STRUCT_VAR_P(st, off) ((void*) ( ((char*)(st)) + (off) ) )
#define STRUCT_VAR_OFFSET(st,p) ((off_t)((char*)&(((st*)0)->p)-(char*)0))

typedef struct smartlist_t {
  void **list;
  int num_used;
  int capacity;
} smartlist_t;

//¹¹ÔìÈÝÆ÷¶ÔÏó
smartlist_t * smartlist_new(void);

//ÊÍ·ÅÈÝÆ÷¶ÔÏó
void smartlist_free(smartlist_t *sl);

//Çå¿ÕÈÝÆ÷
void smartlist_clear(smartlist_t *sl);

//½«Ö¸¶¨ÔªËØÌí¼Óµ½×îºó
void smartlist_add(smartlist_t *sl, void *element);

//½«s2ÖÐµÄËùÓÐÔªËØÌí¼Óµ½s1ÖÐ
void smartlist_add_all(smartlist_t *sl, const smartlist_t *s2);

//ÒÆ³ýÖ¸¶¨ÔªËØ
void smartlist_remove(smartlist_t *sl, const void *element);

//µ¯³ö×îºóÒ»¸öÔªËØ
void *smartlist_pop_last(smartlist_t *sl);

//slÖÐÔªËØµ¹Ðò
void smartlist_reverse(smartlist_t *sl);

//ÒÆ³ýslÖÐÖ¸¶¨µÄ×Ö·û´®
void smartlist_string_remove(smartlist_t *sl, const char *element);

//ÅÐ¶ÏÊÇ·ñ°üº¬Ö¸¶¨ÔªËØ
int smartlist_contains(const smartlist_t *sl, const void *element);

//ÊÇ·ñ°üº¬Ö¸¶¨×Ö·û´®
int smartlist_contains_string(const smartlist_t *sl, const char *element);

//µÃµ½×Ö·û´®ÔÚÈÝÆ÷ÖÐµÄÎ»ÖÃ
int smartlist_string_pos(const smartlist_t *, const char *elt);

//ºöÂÔ´óÐ¡Ð´,ÅÐ¶ÏÊÇ·ñ°üº¬Ö¸¶¨×Ö·û´®
int smartlist_contains_string_case(const smartlist_t *sl, const char *element);

////ÅÐ¶Ï¶ÔÓÚÒÑ¾­ÅÅÐòµÄsl1ºÍsl2´æ·ÅµÄ×Ö·û´®Êý×éÊÇ·ñÏàÍ¬
int smartlist_strings_eq(const smartlist_t *sl1, const smartlist_t *sl2);

//ÅÐ¶ÏÊÇ·ñ°üº¬Ö¸¶¨µÄÄÚ´æµÄÄÚÈÝ,
int smartlist_contains_buf(const smartlist_t *sl, const char *element,int len);

//ÅÐ¶Ï¶ÔÓÚÒÑ¾­ÅÅÐòµÄsl1ºÍsl2´æ·ÅµÄÕûÊýÊý×éÊÇ·ñÏàÍ¬
int smartlist_ints_eq(const smartlist_t *sl1, const smartlist_t *sl2);

//ÅÐ¶Ïsl1ºÍsl2ÊÇ·ñ°üº¬ÏàÍ¬µÄÔªËØ
int smartlist_overlap(const smartlist_t *sl1, const smartlist_t *sl2);

//ÒÆ³ýsl1ÖÐµÄ²»ÔÚsl2ÖÐµÄÔªËØ,±£ÁôÏÂÀ´µÄ¶¼ÄÜÔÚsl2ÖÐ´æÔÚ
void smartlist_intersect(smartlist_t *sl1, const smartlist_t *sl2);

//ÒÆ³ýsl1ÖÐÔÚsl2ÖÐÒ²´æÔÚµÄÔªËØ,±£ÁôÏÂÀ´µÄ¶¼²»ÔÚsl2ÖÐ
void smartlist_subtract(smartlist_t *sl1, const smartlist_t *sl2);

#define smartlist_len(sl) ((sl)->num_used)
#define smartlist_get(sl, idx) ((sl)->list[idx])
#define smartlist_set(sl, idx, val) ((sl)->list[idx] = (val))

static INLINE void smartlist_swap(smartlist_t *sl, int idx1, int idx2)
{
  if (idx1 != idx2) {
    void *elt = smartlist_get(sl, idx1);
    smartlist_set(sl, idx1, smartlist_get(sl, idx2));
    smartlist_set(sl, idx2, elt);
  }
}

//É¾³ýÖ¸¶¨ÏÂ±êµÄÔªËØ
void smartlist_del(smartlist_t *sl, int idx);

//¶ÔÓÚÒÑ¾­ÅÅÐòµÄsl,É¾³ýÖ¸¶¨ÏÂ±êµÄÔªËØ,É¾³ýÖ®ºóÈÔÈ»ÓÐÐò
void smartlist_del_keeporder(smartlist_t *sl, int idx);

//½«Ö¸¶¨ÔªËØ²åÈëµ½Ö¸¶¨ÏÂ±êÎ»ÖÃ
void smartlist_insert(smartlist_t *sl, int idx, void *val);

//¶ÔÈÝÆ÷ÀïÃæµÄÄÚÈÝ½øÐÐÅÅÐò
void smartlist_sort(smartlist_t *sl,
                    int (*compare)(const void **a, const void **b));

//Í³¼Æ³öÏÖ×î¶àµÄÔªËØ,²¢·µ»Ø³öÏÖµÄ´ÎÊý
void *smartlist_get_most_frequent_(const smartlist_t *sl,
                    int (*compare)(const void **a, const void **b),
                    int *count_out);

#define smartlist_get_most_frequent(sl, compare) \
  smartlist_get_most_frequent_((sl), (compare), NULL)

//È¥³ýÈÝÆ÷ÖÐÖØ¸´µÄÔªËØ
void smartlist_uniq(smartlist_t *sl,
                    int (*compare)(const void **a, const void **b),
                    void (*free_fn)(void *elt));

//¶Ô×Ö·û´®Êý×é½øÐÐÅÅÐò
void smartlist_sort_strings(smartlist_t *sl);

//°´ÕÕÖ¸Õë´óÐ¡½øÐÐÅÅÐò
void smartlist_sort_pointers(smartlist_t *sl);

//Í³¼Æ³öÏÖ×î¶à´ÎµÄ×Ö·û´®
char *smartlist_get_most_frequent_string(smartlist_t *sl);

//Í³¼Æ³öÏÖ×î¶à´ÎµÄ×Ö·û´®,²¢·µ»Ø³öÏÖµÄ´ÎÊý
char *smartlist_get_most_frequent_string_(smartlist_t *sl, int *count_out);


//È¥³ýÖØ¸´³öÏÖµÄ×Ö·û´®
void smartlist_uniq_strings(smartlist_t *sl);

//¶ÔÓÚÅÅÐòµÄsl,Í¨¹ýkey¹Ø¼ü×Ö½øÐÐ¶þ·Ö²éÕÒ,½«ÕÒµ½µÄÔªËØÍ¨¹ýmember·µ»Ø
void *smartlist_bsearch(smartlist_t *sl, const void *key,
                        int (*compare)(const void *key, const void **member));

//¶ÔÓÚÅÅÐòµÄsl,Í¨¹ýkey¹Ø¼ü×Ö½øÐÐ¶þ·Ö²éÕÒ,·µ»ØÕÒµ½µÄÔªËØÔÚÈÝÆ÷ÖÐµÄÎ»ÖÃ
int smartlist_bsearch_idx(const smartlist_t *sl, const void *key,
                          int (*compare)(const void *key, const void **member),
                          int *found_out);

//½«ÔªËØ¼ÓÈëµ½¶ÑÖÐ
void smartlist_pqueue_add(smartlist_t *sl,
                          int (*compare)(const void *a, const void *b),
                          int idx_field_offset,
                          void *item);

//ÒÆ³ý¶Ô¶¥ÔªËØ
void *smartlist_pqueue_pop(smartlist_t *sl,
                           int (*compare)(const void *a, const void *b),
                           int idx_field_offset);

//ÒÆ³ý¶ÑÖÐÖ¸¶¨ÔªËØ
void smartlist_pqueue_remove(smartlist_t *sl,
                             int (*compare)(const void *a, const void *b),
                             int idx_field_offset,
                             void *item);


#define SPLIT_SKIP_SPACE   0x01
#define SPLIT_IGNORE_BLANK 0x02
#define SPLIT_STRIP_SPACE  0x04


//ÓÃsep·Ö¸î×Ö·û´®str,½«·Ö¸î²úÉúµÄ×Ö·û´®Êý×é´æÈëµ½slÖÐ
int smartlist_split_string(smartlist_t *sl, const char *str, const char *sep,
                           int flags, int max);

//½«slÖÐµÄ×Ö·û´®Êý×éÁ¬½Ó³ÉÎªÒ»¸öÐÂµÄ×Ö·û´®
char * smartlist_join_strings(smartlist_t *sl, const char *join, int terminate,
                              size_t *len_out);

//½«slÖÐµÄ×Ö·û´®Êý×éÁ¬½Ó³ÉÎªÒ»¸öÐÂµÄ×Ö·û´®
char *smartlist_join_strings2(smartlist_t *sl, const char *join,
                              size_t join_len, int terminate, size_t *len_out);


#define STMT_NIL (void)0
#define STMT_VOID(a) while (0) { (void)(a); }

#ifdef __GNUC__
    #define STMT_BEGIN (void) ({
    #define STMT_END })
#elif defined(sun) || defined(__sun__)
    #define STMT_BEGIN if (1) {
    #define STMT_END } else STMT_NIL
#else
    #define STMT_BEGIN do {
    #define STMT_END } while (0)
#endif


#define SMARTLIST_FOREACH_BEGIN(sl, type, var)  \
  STMT_BEGIN                                                           \
    int var ## _sl_idx,var## _cur_del=0,var ## _sl_len=(sl)->num_used; \
    type var;                                                          \
    for (var ## _sl_idx = 0; var ## _sl_idx < var ## _sl_len;          \
         ++var ## _sl_idx) {                                           \
         var ## _cur_del=0;                                            \
         var = (type)(sl)->list[var ## _sl_idx];



#define SMARTLIST_REMOVE_CUR(var) \
    var##_cur_del=1;



#define SMARTLIST_FOREACH_END(var)              \
    var = NULL;                                 \
  }                                             \
  STMT_END




#define SMARTLIST_FOREACH(sl, type, var, cmd)                   \
  SMARTLIST_FOREACH_BEGIN(sl,type,var) {                        \
    cmd;                                                        \
    if(var ##_cur_del){                                         \
        smartlist_remove((sl),var);                             \
        --var ## _sl_idx;                                       \
    }                                                           \
    var ## _sl_len=(sl)->num_used;                              \
  } SMARTLIST_FOREACH_END(var)

#ifdef __cplusplus
}
#endif

#endif
