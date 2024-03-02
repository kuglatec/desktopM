#include <gtk/gtk.h>
#include <dirent.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#define WIDTH 30
#define HEIGHT 10




int startx = 0;
int starty = 0;


typedef struct Xsession {
	char name[32];
	char path[250];
	char exec[250];
} Xsession;


Xsession Sessions[50];

int entryNum() {
  DIR *d;
  struct dirent *dir;
  d = opendir("/usr/share/xsessions");
  int counter = 0;
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_name[0] != '.')
        counter++;
    }
    closedir(d);
  }
  return(counter);
}


void print_menu(WINDOW *menu_win, int highlight, Xsession Sessions[50]);


void startde (char *path, char *username) {
  char cmd[50];
  struct passwd *user_info = getpwnam(username);
  uid_t target_uid = user_info->pw_uid;
  setuid(target_uid);
  snprintf(cmd, 50, "startx %s", path);
  char homedir[250];
  snprintf(homedir, 250, "/home/%s", username);
  setenv("HOME", homedir, 1);
  
  system("%s". cmd);
}


int main()
{	int nof;
	nof = entryNum();
	char user[32];
  printf("Enter Username: ");
  scanf("%32s", &user);
  Xsession Sessions[nof];
	char filenames[nof][15];
	DIR *dir;
	int i = 0;
    	struct dirent *entry;
    	dir = opendir("/usr/share/xsessions");
	while ((entry = readdir(dir)) != NULL && i < nof) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            sprintf(Sessions[i].name, "%s\n", entry->d_name);
	    i++;
        }
    }

	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;
	for (i = 0; i < nof; i++) {
	snprintf(Sessions[i].path, 250, "/usr/share/xsessions/%s", Sessions[i].name);
  	printf("%s\n", Sessions[i].path); 
	}
	GKeyFile *key_file;
    	GError *error;
    	gsize length;
	gchar *str_value;
    	key_file = g_key_file_new();
    	guint group, key;
	char *strr = Sessions[1].path;
	size_t actualLength = strlen(strr) - 1;
    	char trimmedArray[actualLength];
    	strncpy(trimmedArray, strr, actualLength);
	trimmedArray[actualLength] = '\0';
	for (i = 0; i < nof; i++){
		

		char *strr = Sessions[i].path;
		size_t actualLength = strlen(strr) - 1;
	    	char trimmedArray[actualLength];
	    	strncpy(trimmedArray, strr, actualLength);
		trimmedArray[actualLength] = '\0';		

		error = NULL;
		if(!g_key_file_load_from_file(key_file,
                trimmedArray,
                G_KEY_FILE_KEEP_COMMENTS | 
                G_KEY_FILE_KEEP_TRANSLATIONS,
                &error))
    {
        g_debug("%s", error->message);
	printf("\n%s", error->message);
    }
    else
    {
        str_value = g_key_file_get_string(key_file,
                                          "Desktop Entry",
                                          "Exec",
                                          &error);
	strcpy(Sessions[i].exec, str_value);
	free(str_value);

	
    }
    if (Sessions[i].exec[0] != '/')
    {
      char buff[250];
      sprintf(buff, "/bin/%s", Sessions[i].exec);
      strcpy(Sessions[i].exec, buff);
      printf("\n%s", Sessions[i].exec);
    }
  
  }
	initscr();
	clear();
	noecho();
	cbreak(); 
	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;

	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	print_menu(menu_win, highlight, Sessions);
	while(1)
	{	c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = nof;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == nof)
					highlight = 1;
				else
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				refresh();
				break;
		}
		print_menu(menu_win, highlight, Sessions);
		if(choice != 0)	 
			break;
	}
	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, Sessions[choice - 1].name);
	clrtoeol();
	refresh();
	startde(Sessions[choice].path, user);
	getch();
	endwin(); 
	return 0;
}

void print_menu(WINDOW *menu_win, int highlight, Xsession Sessions[50])
{
	int x, y, i;

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
  int nof = entryNum();
	for(i = 0; i < nof; ++i)
	{	if(highlight == i + 1)
		{	wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", Sessions[i].name);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", Sessions[i].name);
		++y;
	}
	wrefresh(menu_win);
}
