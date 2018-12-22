/* AP     18-apr-05 bakery.h            */
/*    declarations for an implementation of     */
/*    sections based on bakery algorithm    */

# ifndef __bakery_h
# define __bakery_h

typedef struct _crit_sec * MY_LPCRITICAL_SECTION;

void My_InitializeCriticalSection( MY_LPCRITICAL_SECTION * sec );
void My_DeleteCriticalSection( MY_LPCRITICAL_SECTION sec );

void My_EnterCriticalSection( int ThId, MY_LPCRITICAL_SECTION sec );
void My_LeaveCriticalSection( int ThId, MY_LPCRITICAL_SECTION sec );

# endif