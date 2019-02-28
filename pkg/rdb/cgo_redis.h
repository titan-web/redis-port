#include <redis/src/server.h>

void initRedisServer(char *config);

/* API of Redis Rio/Rdb */

#define REDIS_RIO_BUFSIZE (1024 * 16)

typedef struct {
  rio rdb;
  size_t pos, end;
  char buf[REDIS_RIO_BUFSIZE];
} redisRio;

void redisRioInit(redisRio *p);

int redisRioRead(redisRio *p, void *buf, size_t len);
int redisRioLoadLen(redisRio *p, uint64_t *len);
int redisRioLoadType(redisRio *p, int *typ);
int redisRioLoadTime(redisRio *p, time_t *val);
int redisRioLoadTimeMillisecond(redisRio *p, long long *val);

void *redisRioLoadObject(redisRio *p, int typ);
void *redisRioLoadStringObject(redisRio *p);

inline uint64_t redisRioChecksum(redisRio *p) { return p->rdb.cksum; }

/* API of Sds */
typedef struct {
  void *ptr;
  size_t len;
  long long val;
  double score;
} redisSds;

inline void redisSdsFreePtr(void *ptr) { sdsfree(ptr); }

typedef struct {
  void *iter;
  struct {
    size_t len;
    redisSds buf[512];
  } slice;
  size_t index;
  size_t (*load)(void *iter, redisSds *buf, size_t len);
} redisTypeIterator;

void redisTypeIteratorLoad(redisTypeIterator *p);

/* API of redis Object */
int redisObjectType(void *obj);
int redisObjectEncoding(void *obj);
int redisObjectRefCount(void *obj);

void redisObjectIncrRefCount(void *obj);
void redisObjectDecrRefCount(void *obj, int lazyfree);

void redisObjectCreateDumpPayload(void *obj, redisSds *p);
void *redisObjectDecodeFromPayload(void *buf, size_t len);

/* API of redis String */
size_t redisStringObjectLen(void *obj);
void redisStringObjectLoad(void *obj, redisSds *sds);

/* API of redis List */
size_t redisListObjectLen(void *obj);
redisTypeIterator *redisListObjectNewIterator(void *obj);
void redisListIteratorRelease(redisTypeIterator *p);

/* API of redis Hash */
size_t redisHashObjectLen(void *obj);
redisTypeIterator *redisHashObjectNewIterator(void *obj);
void redisHashIteratorRelease(redisTypeIterator *p);

/* API of redis Zset */
size_t redisZsetObjectLen(void *obj);
redisTypeIterator *redisZsetObjectNewIterator(void *obj);
void redisZsetIteratorRelease(redisTypeIterator *p);

/* API of redis Set */
size_t redisSetObjectLen(void *obj);
redisTypeIterator *redisSetObjectNewIterator(void *obj);
void redisSetIteratorRelease(redisTypeIterator *p);

/* API of redis zmalloc */
extern size_t zmalloc_used_memory(void);
extern size_t zmalloc_memory_size(void);
extern size_t zmalloc_get_rss(void);
