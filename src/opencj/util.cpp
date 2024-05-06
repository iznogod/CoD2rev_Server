#include "opencj.h"

void CJ_iprintln(int clientNum, char *message)
{
    char string[MAX_STRING_CHARS];
    snprintf(string, MAX_STRING_CHARS, "f \"%s\"", message);
    SV_GameSendServerCommand(clientNum, SV_CMD_CAN_IGNORE, string);
}