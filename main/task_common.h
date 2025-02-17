#ifndef TASK_COMMON_H
#define TASK_COMMON_H


#define CONFIG_LOG_MAXIMUM_LEVEL    3

#define NAMESPACE_DEVICE_INFO "DEVICE_INFO"
#define KEY_TYPE "CardType"
#define KEY_UID "UID"
#define KEY_ATQA "ATQA_KEY"
#define KEY_SAK "SAK_KEY"

char g_uid[20];
char g_atqa[20];
char g_sak[20];
char g_type[50];
#endif