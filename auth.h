#ifndef AUTH_H
#define AUTH_H

#define AUTH_MAX_USERS 100U
#define AUTH_FIELD_LEN 30U
#define AUTH_USER_FILE "users.dat"

/* Fixed-size static store (no heap allocation) - deliberately
   avoids dynamic memory for user credentials, both to reduce
   allocator churn and because MISRA C discourages heap use
   where a bounded static structure works just as well. */

/* Loads existing users from AUTH_USER_FILE into memory (if the
   file does not exist yet, this is not an error - it just
   means no users are registered yet). Call once at startup. */
void authInit(void);

/* Registers a new user. Returns 1 on success, 0 if the
   username already exists, the store is full, username or
   password is empty, or the username/password exceed
   AUTH_FIELD_LEN - 1 characters. Persists to AUTH_USER_FILE
   on success. */
int authRegister(const char *username, const char *password);

/* Returns 1 if username/password match a registered user,
   0 otherwise. */
int authLogin(const char *username, const char *password);

#endif
