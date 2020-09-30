#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct user {
	char username[60];
	char password[60];
	int  state;
};
typedef struct user USER;

void add_user_information(USER *user, char *str);
int main() {
	FILE *fp;
	fp = fopen("data.txt", "r");
	char str[60];
	if(fp == NULL) {
		puts("Error! Read from file with error.\n");
		return 0;
	}
	USER user_data[10];
	int j = 0;
	while(fgets(str, 60, fp) != NULL) {

		add_user_information(&user_data[j++], str);
	}

	for (int i = 0; i< j; i++) {
		printf("info user %d: %s - %s - %d\n"
		, i, user_data[i].username, user_data[i].password,
		user_data[i].state);
	}
	
	return 0;
			
}

void add_user_information(USER *user, char *str) {
	char s[2] = " ";
	char *token;
	token = strtok(str, s);
	memcpy(user->username, token, strlen(token));
	token = strtok(NULL, s);
	memcpy(user->password, token, strlen(token));
	token = strtok(NULL, s);
	user->state = atoi(token);

}