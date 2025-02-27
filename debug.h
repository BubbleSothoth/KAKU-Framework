#ifndef DEBUG_H
#define	DEBUG_H

#include <stdio.h>


#ifndef __FUNCTION__

#define __FUNCTION__ "UNDEFINED"

#endif // !__FUNCTION__

#define _SHOW_LOG(type,mess) printf("[%s] %s %s [%s : %d] %s : %s\n",type,__DATE__,__TIME__,__FILE__,__LINE__,__FUNCTION__,mess)
#define _SHOW_DEBUG(type,mess) printf("[%s] %s\n",type,mess)

#define LG_ERROR(mess) _SHOW_LOG("ERROR",mess)
#define LG_WARNING(mess) _SHOW_LOG("WARNING",mess)
#define LG_INFO(mess) _SHOW_LOG("INFO",mess)
#define LG_CUST(debug_type,mess) _SHOW_LOG(debug_type,mess)
#define LG_LOG(mess) _SHOW_LOG("LOG",mess)

#define DB_ERROR(mess) _SHOW_DEBUG("ERROR",mess)
#define DB_WARNING(mess) _SHOW_DEBUG("WARNING",mess)
#define DB_INFO(mess) _SHOW_DEBUG("INFO",mess)
#define DB_CUST(debug_type,mess) _SHOW_DEBUG(debug_type,mess)

#endif // !DEBUG_H