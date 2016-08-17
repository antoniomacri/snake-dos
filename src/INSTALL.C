#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <conio.h>
#include <sys\stat.h>
#include <io.h>
#include <errno.h>


int exist(const char *filename);
int fcopy(const char *source, const char *destination);
int noRdOnlyAttr (const char * filename);
int installa(void);
int disinstalla(void);


const char snakeFolder[]  =  "C:\\Snake";

const char srcSnakeFileName[] = "snake.exe";
const char snakeFileName[] = "C:\\SNAKE\\snake.exe";

const char cfgFileName[]  =  "C:\\SNAKE\\snakecfg.cfg";
const char savedGame[]   =   "C:\\SNAKE\\savedGm.snk";

const char srcLink[] = "snake.pif";

const char win2k[130]  =  "C:\\DOCUME~1\\ALLUSE~1\\DESKTOP";
const char win98[130]  =  "C:\\WINDOWS\\DESKTOP";
const char *desktopLink;


int main()  {
	char titolo[] = "Programma di installazione/disinstallazione di Snake (per Win)";

	clrscr();
	gotoxy(40-strlen(titolo)/2, 2);
	printf(titolo);
	printf("\n%80s", "by SnakePlissken   ");

	desktopLink = (exist(win2k) ? win2k : win98);
	strcat((char*)desktopLink, (char*)"\\Snake.pif");

	return ((exist(snakeFileName)) ? disinstalla() : installa());
}


int disinstalla()  {
	char c;

	printf("\n\nPer disinstallare Snake premi Invio...\n");
	do
		c=getch();
	while (c!=0x0D && c!=0x1B);

	if (c==0x1B)
		return(1);

	printf("\n\nE' in corso la disinstallazione di Snake...\n");

	noRdOnlyAttr(cfgFileName);
	unlink(cfgFileName);
	unlink(savedGame);

	unlink(snakeFileName);

	if (rmdir(snakeFolder) || unlink(desktopLink))  {
		printf("\n\nErrore!");
		printf("\n\nImpossibile disinstallare correttamente Snake.");
		getch();
		return(-1);
	}

	printf("\n\nDisinstallazione completata!\n\n");
	getch();
	return(0);
}


int installa()  {
	char c;
	int flag = 0;

	printf("\n\nPer installare Snake premi Invio...\n");
	do
		c=getch();
	while (c!=0x0D && c!=0x1B);

	if (c==0x1B)
		return(1);

	printf("\n\nE' in corso l'installazione di Snake...\n");

	if (!exist(snakeFolder))  {
		flag = mkdir(snakeFolder);
		if(flag)  {
			perror("mkdir()");
			printf("Impossibile creare la directory di installazione:\n%s\n", snakeFolder);
		}
	}

	if (!flag)  {
		flag = fcopy(srcSnakeFileName, snakeFileName);
		if (flag)  {
			if(flag == -1)
				printf("\nImpossibile trovare \"%s\"!\n", srcSnakeFileName);
			else
				printf("\nImpossibile creare \"%s\"!\n", snakeFileName);
		}
	}

	if (!flag)  {
		flag = fcopy(srcLink, desktopLink);
		if (flag)  {
			if(flag == -1)
				printf("\nImpossibile trovare \"%s\"!\n", srcLink);
			else
				printf("\nImpossibile creare \"%s\"!\n", desktopLink);
		}
	}

	if (flag)  {
		printf("\nPremere un tasto per uscire...\n");
		getch();
		return(-1);
	}

	printf("\n\nInstallazione completata!\n\n");
	printf("Per avviare Snake fare clic sull'icona presente sul desktop.\n");
	getch();
	return(0);
}


int fcopy(const char *source, const char *destination)  {
	FILE *fsource, *fdestination;
	unsigned char buffer[1025];
	int count;

	if((fsource = fopen(source, "rb")) == NULL)
		return (-1);
	if((fdestination = fopen(destination, "wb")) == NULL)  {
		fclose(fsource);
		return (1);
	}

	while((count = fread(buffer, 1, 1024, fsource)) != 0)
		fwrite(buffer, 1, count, fdestination);

	fcloseall();
	return (0);
}


int exist(const char *filename)  {
  return (access(filename, 0) == 0);
}


int noRdOnlyAttr (const char * filename)  {
	struct stat stbuf;

	if (stat(filename, &stbuf) != 0)	//  get file information
		return(-1);

	if (stbuf.st_mode & S_IWRITE)		// il file non è solo-lettura
		return(0);				// quindi è già a posto

	else					// se il file è solo-lettura
		return(chmod(filename, S_IREAD|S_IWRITE));	// lo cambia
		// e ritorna zero in caso di successo, un valore diverso
		// se si verifica un errore (allo stesso modo fa la chmod()
}