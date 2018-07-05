#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct node_ node_t;
struct node_ {
	char *name; // 歌曲名字
	node_t *prev;
	node_t *next;
};

node_t *head = NULL;
node_t *cur = NULL; // 指向当前播放的歌
int first    = 1;   // 是否第一次播放

enum { STOP, PAUSE, PLAY};
int status = STOP;

void list_init( void )
{
	head = malloc(sizeof(node_t));
	memset(head, 0x00, sizeof(node_t));

	head->next = head->prev = head;
}

void list_insert( const char *name ) {
	node_t *p = malloc(sizeof(node_t));
	memset(p, 0x00 ,sizeof(node_t));

	p->name = malloc(strlen(name)+1);
	strcpy(p->name, name);

	p->next = head->next;
	p->prev = head;
	head->next->prev = p;
	head->next = p;
}

void list_show( void )
{
	node_t *p = head->next;
	
	while ( p != head ) {
		printf("%s ", p->name);
		if ( p == cur )
			printf(" <==cur");
		printf("\n");
		p = p->next;
	}
}

void load_music( const char *path ) {
	DIR *pdir = opendir(path);
	if ( pdir == NULL ) perror("opendir"), exit(1);

	struct dirent *p = NULL;
	while ( (p=readdir(pdir)) != NULL ) {
		if ( p->d_name[0] == '.' ) 
				continue;
		list_insert(p->d_name);
	}

	closedir(pdir);
}

int menu( void ) {
	printf("+---------------------------+\n");
	printf(" 1. play/pause\n");
	printf(" 2. next\n");
	printf(" 3. prev\n");
	printf(" 4. stop\n");
	printf(" 0. exit\n");
	printf("+---------------------------+\n");
	list_show();
	
	int choose = 4;
	do {
		printf(" > ");
		scanf("%d", &choose);
		if ( choose>=0 && choose<=4 )
			break;
		printf("choose invalid\n");
		while ( getchar() != '\n' ) ;
	} while ( 1 );

	return choose;
}

void playPause()
{
	if ( first == 1 ) {
		char buf[1024] = {};
		sprintf(buf, "madplay -o wav:- /home/lxy/Desktop/mp3lib/shixi/bin/music/%s 2> /dev/null | aplay 2> /dev/null &", cur->name);
		system(buf);
		first = 0;
		status = PLAY;
	} else {
		if ( status == PLAY ) {
			system("killall -SIGSTOP aplay");
			status = PAUSE;
		} else if ( status == PAUSE ) {
			system("killall -SIGCONT aplay");
			status = PLAY;
		}
	}	
}

void stop()
{
	system("killall -SIGKILL aplay");
	first = 1;
}

void next()
{
	stop();
	cur = cur->next;
	if ( cur == head )
		cur = cur->next;
	playPause();
}

void prev()
{
	stop();
	cur = cur->prev;
	if ( cur == head )
		cur = cur->prev;
	playPause();
}

int main( int argc, char *argv[] ) {
	list_init();
	load_music("/home/lxy/Desktop/mp3lib/shixi/bin/music");
	if ( head->next != head )
		cur = head->next;

	do {
		int choose = menu();
		switch ( choose ) {
		case 1:
			playPause();
			break;
		case 2:
			next();
			break;
		case 3:
			prev();
			break;
		case 4:
			stop();
			break;
		case 0:
			printf("谢谢欣赏\n");
			system("killall -SIGKILL aplay");
			exit(0);
			break;
		default:
			// do nothing
			break;
		}
	} while ( 1 );

	return 0;
}

