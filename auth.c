#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "../logger/logger.h"

typedef struct
{
    char username[AUTH_FIELD_LEN];
    char password[AUTH_FIELD_LEN];
} AuthUser;

static AuthUser users[AUTH_MAX_USERS];
static unsigned int userCount = 0U;

void authInit(void)
{
    userCount = 0U;

    FILE *fp = fopen(AUTH_USER_FILE, "r");

    if(fp == NULL)
    {
        /* No existing user file is a normal first-run state,
           not an error. */
        logInfo("authInit: no existing user file (first run)");
        return;
    }

    char line[2U * AUTH_FIELD_LEN + 4U];

    while((userCount < AUTH_MAX_USERS) &&
          (fgets(line, sizeof(line), fp) != NULL))
    {
        char uname[AUTH_FIELD_LEN];
        char pword[AUTH_FIELD_LEN];

        int matched = sscanf(line, "%29s %29s", uname, pword);

        if(matched == 2)
        {
            strncpy(users[userCount].username, uname, AUTH_FIELD_LEN - 1U);
            users[userCount].username[AUTH_FIELD_LEN - 1U] = '\0';

            strncpy(users[userCount].password, pword, AUTH_FIELD_LEN - 1U);
            users[userCount].password[AUTH_FIELD_LEN - 1U] = '\0';

            userCount++;
        }
        else
        {
            logWarning("authInit: skipped malformed line in user file");
        }
    }

    if(fclose(fp) != 0)
    {
        logWarning("authInit: fclose failed on user file");
    }

    logInfo("authInit: user store loaded");
}

static int usernameExists(const char *username)
{
    for(unsigned int i = 0U; i < userCount; i++)
    {
        if(strcmp(users[i].username, username) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int authRegister(const char *username, const char *password)
{
    if((username == NULL) || (password == NULL))
    {
        logError("authRegister: NULL argument");
        return 0;
    }

    size_t uLen = strlen(username);
    size_t pLen = strlen(password);

    if((uLen == 0U) || (pLen == 0U))
    {
        printf("Username and password cannot be empty.\n");
        logWarning("authRegister: empty username or password");
        return 0;
    }

    if((uLen > (AUTH_FIELD_LEN - 1U)) || (pLen > (AUTH_FIELD_LEN - 1U)))
    {
        printf("Username/password too long (max %u characters).\n", AUTH_FIELD_LEN - 1U);
        logWarning("authRegister: field too long");
        return 0;
    }

    if(userCount >= AUTH_MAX_USERS)
    {
        printf("User store full - cannot register more users.\n");
        logError("authRegister: user store full");
        return 0;
    }

    if(usernameExists(username) == 1)
    {
        printf("Username already exists. Please choose another.\n");
        logWarning("authRegister: duplicate username");
        return 0;
    }

    strncpy(users[userCount].username, username, AUTH_FIELD_LEN - 1U);
    users[userCount].username[AUTH_FIELD_LEN - 1U] = '\0';

    strncpy(users[userCount].password, password, AUTH_FIELD_LEN - 1U);
    users[userCount].password[AUTH_FIELD_LEN - 1U] = '\0';

    userCount++;

    /* Persist the whole store. For AUTH_MAX_USERS = 100 this
       is a small, bounded write - simpler and safer than
       appending (avoids partial-write corruption on repeated
       runs) at negligible cost. */
    FILE *fp = fopen(AUTH_USER_FILE, "w");

    if(fp == NULL)
    {
        printf("Warning: registered in memory, but could not save to disk.\n");
        logError("authRegister: fopen for write failed");
        return 1;
    }

    for(unsigned int i = 0U; i < userCount; i++)
    {
        if(fprintf(fp, "%s %s\n", users[i].username, users[i].password) < 0)
        {
            logError("authRegister: fprintf failed while saving user store");
        }
    }

    if(fclose(fp) != 0)
    {
        logWarning("authRegister: fclose failed after saving user store");
    }

    logInfo("authRegister: user registered and persisted");
    return 1;
}

int authLogin(const char *username, const char *password)
{
    if((username == NULL) || (password == NULL))
    {
        return 0;
    }

    for(unsigned int i = 0U; i < userCount; i++)
    {
        if((strcmp(users[i].username, username) == 0) &&
           (strcmp(users[i].password, password) == 0))
        {
            logInfo("authLogin: successful login");
            return 1;
        }
    }

    logWarning("authLogin: failed login attempt");
    return 0;
}
