#include "string.h"

char g_uid[20];
char g_atqa[20];
char g_sak[20];

// NVS namespace và keys
#define NVS_NAMESPACE "rfid_data"
#define NVS_KEY_UID "uid"
#define NVS_KEY_ATQA "atqa"
#define NVS_KEY_SAK "sak"