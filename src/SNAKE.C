/*************************************************
Snake Nokia dimensione schermo 13x23
Snake PC                       44x78

// sembra fatto apposta, ma le dimensioni sono
// (quasi) perfettamente proporzionali:
// 13 : 44 = 23 : 78

// 3,388   // rapporto dimensioni Snake PC/Nokia
*************************************************/




//  I N C L U D E S:   L I B R E R I E   S T A N D A R D
#include <stdio.h>		// FILE e gli streams, fopen(), fread(), fwrite(), fclose(), fgets()
#include <conio.h>		// per i colori, kbhit(), getch(), _setcursortype()
#include <dos.h>		// struct time, gettime(), sound(), delay(), nosound(), geninterrupt() (in extio.def)
#include <stdlib.h>		// random()
#include <string.h>		// strlen(), strcpy()
#include <mem.h>		// setmem()
#include <time.h>		// randomize()
#include <io.h>			// chmod() (in funzioni.def)
#include <sys\stat.h>		// struct stat, stat(), S_IREAD, S_IWRITE (in funzioni.def)
#include <stdarg.h>


//  I N C L U D E S:   L I B R E R I E   M I E
#include "gckeys.h"
#include "extio.def"
#include "funzioni.def"


//  P R O T O T I P I
void loadCfg(void);
int  controllaCfg(void);
void saveSettings(void);
void displayTime(void);
void gioca(void);
void playSnake(struct Snake *snake, int game);
int  caricaPartita(struct Snake *snake);
int  salvaPartita(struct Snake *snake);
int  getRecordAndSave(struct Snake *snk);
void record (int);
void menu_OpzioniGioco(void);
void subMenu_Livelli(void);
void subMenu_Labirinti(void);
void visualizzaLabirinto(int lab);
void menu_OpzioniProg(void);
void subMenu_BackGcolor(void);
void subMenu_ForeGcolor(void);
void informazioni (void);


//  C O S T A N T I   M A N I F E S T E
#define VER		"1.6"
#define FALSE		0
#define TRUE		(!FALSE)
#define MAXSNAKELENGTH	300		// lunghezza massima che può raggiungere il serpente
#define MINSNAKELENGTH	5		// lunghezza iniziale del serpente
#define MAXRECORDS	15		// numero massimo di records registrabili nel cfgfile
#define STARTNEWGAME	2		// -\_  valori passati a playSnake() per stabilire se il
#define RELOADGAME	3		// -/   gioco va caricato o se ne va iniziato uno nuovo


//  V A R I A B I L I
const char titolo[] = "Snake for PC v"VER" - by SnakePlissken";	// Titolo del prog
const char cfgFileName[] = "C:\\SNAKE\\snakecfg.cfg";	// File di configurazione
const char savedGame[] = "C:\\SNAKE\\savedGm.snk";	// Nomefile del gioco salvato o da salvare
char buffer[30];						// stringa-buffer utilizzata varie volte
int color;							// Colore (o meglio, attributi) dello schermo
int dialogBoxColor=makecolor(LIGHTGRAY, BLACK);			// Colore (o meglio, attributi) dei box di dialogo
struct {						// Struttura memorizzata nel file di configurazione
	int livello;					//   Ultimo livello utilizzato
	int labirinto;					//   Ultimo labirinto utilizzato
	int foreGroundColor, backGroundColor;		//   Colori impostati per lo sfondo e per il testo
	struct {				// Struttura che contiene i records raggiunti
		int punti;			//   Punteggio
		int livello;			//   Livello utilizzato nella partita
		int labirinto;			//   Labirinto impostato per la partita
		int ciboRaccolto;		//   Cibo raccolto
		unsigned long durataGioco;	//   Durata del gioco (in centesimi di secondo)
		char nome[31];			//   Nome del giocatore
	} records[MAXRECORDS];
}  settings;
struct Snake  {			// Struttura usata nela gestione, nel salvataggio e nel caricamento di una partita
	int livello;		//    Livello impostato per la partita
	int labirinto;		//    Labirinto impostato per la partita
	int snklen;		//    Lunghezza del corpo del serpente
	int x[MAXSNAKELENGTH],	//    Coordinate del serpente:
	    y[MAXSNAKELENGTH];	//       (x[0],y[0])=coda;  (x[snklen],y[snklen])=testa
	int testax, testay;	//    Coordinate della testa del serpente
	int ciboRaccolto;	//    Contatore del cibo raccolto
	int cibox, ciboy;	//    Coordinate del cibo
	int ciboExtrax, ciboExtray;	// coord. del primo carattere del ciboExtra
	int tipoCiboExtra;	//    Cibo extra dell'array ciboExtra (presente in playSnake()) visualizzato
	int mosse;		//    Mosse della serpe che restano per prendere il ciboExtra.
				// In pratica Š una sorta di timer.
	int ciboExtraPresente;		// Se c'Š il ciboExtra == TRUE else FALSE
	int punti;		//    Punteggio corrente
	unsigned long durataGioco;	// In centesimi di secondo
};



//  I N I Z I O   C O D I C E   F U N Z I O N I

//  DEFINIZIONE MAIN()
int main()  {
	register i=0;
	int currmenu=0, prevmenu=0;
	char scelta;
	const char *menu[]  = {"Gioca a Snake",
			       "Record",
			       "Opzioni di Gioco",
			       "Opzioni del Programma",
			       "Informazioni su Snake",
			       "Esci"};

	screenmode80x50();
	_setcursortype(_NOCURSOR);

	loadCfg();

	while(1)  {
		cls(color);
		cornice(1, 4, 80, 49, color);

		dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

		dv_printxy(33, 13, "Menu Principale", color);

		for (i=0; i<=5; i++)
			dv_printxy(15, 18+i*3, menu[i], color);

		do  {
			dv_printxy(15, 18+prevmenu*3, menu[prevmenu], color);
			dv_printxy(15, 18+currmenu*3, menu[currmenu], ~color);

			do  {
				displayTime();
				scelta=getch();
			}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
				  scelta!=GC_ESC && scelta!=GC_ENTER);
			prevmenu=currmenu;
			switch(scelta)  {
				case GC_UP:
					if (currmenu==0) currmenu=5;
					else		 currmenu--;
					break;
				case GC_DOWN:
					if (currmenu==5) currmenu=0;
					else		 currmenu++;
			}
			if (scelta==GC_ESC)	currmenu=5;
		}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

		switch(currmenu)  {
			case 0:
				gioca();		break;
			case 1:
				record(1);		break;
			case 2:
				menu_OpzioniGioco();	break;
			case 3:
				menu_OpzioniProg();	break;
			case 4:
				informazioni();		break;
			case 5:
				saveSettings();
				return (0);
		}
	}
}


//  DEFINIZIONE FUNZIONI

// loadCfg()	carica i parametri di impostazione del gioco eventualmente
//		presenti nel file di configurazione cfgFileName.
//		Se il file non esiste, inizializza i parametri fondamentali
//		del gioco (tra quelli presenti nella struct settings).
void loadCfg(void)  {
	FILE *cfg;
	register i;

	if((cfg=fopen(cfgFileName, "rb"))!=NULL)  {
		// 1. Il file di configurazione esiste gi…, quindi carica i parametri
		fread(&settings.livello, sizeof(int), 1, cfg);
		fread(&settings.labirinto, sizeof(int), 1, cfg);
		fread(&settings.foreGroundColor, sizeof(int), 1, cfg);
		fread(&settings.backGroundColor, sizeof(int), 1, cfg);
		for (i=0;i<MAXRECORDS; i++)  {
			fread(&(settings.records[i].punti), sizeof(int), 1, cfg);
			fread(&(settings.records[i].livello), sizeof(int), 1, cfg);
			fread(&(settings.records[i].labirinto), sizeof(int), 1, cfg);
			fread(&(settings.records[i].ciboRaccolto), sizeof(int), 1, cfg);
			fread(&(settings.records[i].durataGioco), sizeof(unsigned long), 1, cfg);
			fread(settings.records[i].nome, sizeof(char), 31, cfg);
		}
		fclose(cfg);

		color = makecolor(settings.backGroundColor, settings.foreGroundColor);

		// faccio dei controlli sul cfgfile appena caricato nella
		// struct settings
		if(controllaCfg()==0)	// il cfgfile non ha nessun problema
			return;
	}

	// (qua non c'Š un else ma Š come se ci fosse, perch‚ la if qua sopra
	// se tutto Š a posto termina con un return). Quindi:

	// 2. altrimenti (il file non esiste oppure Š danneggiato) inizializzo i
	// parametri (il file sar… creato e i dati salvati in esso alla chiusura del programma)

	// azzero tutta la struct...
	setmem(&settings, sizeof(settings), '\0');
	// ...e inizializzo i parametri fondamentali
	settings.livello = 1;
	settings.labirinto = 0;
	settings.foreGroundColor = WHITE;
	settings.backGroundColor = BLUE;

	// infine, memorizzo i colori del testo e dello sfondo nella variabile globale color
	color = makecolor(settings.backGroundColor, settings.foreGroundColor);
}


int controllaCfg(void)  {
	register int i, j;
	int danneggiato = FALSE;
	char c;

	// Faccio dei controlli sul cfgfile
	danneggiato =  ((settings.livello < 1) || (settings.livello > 9)  ||
			(settings.labirinto < 0) || (settings.labirinto > 5));

	for (i=0; (danneggiato==FALSE) && (i<MAXRECORDS) && (settings.records[i].nome[0]); i++)  {
		if ((settings.records[i].livello < 1) || (settings.records[i].livello > 9) ||
		    (settings.records[i].labirinto < 0) || (settings.records[i].labirinto > 5) ||
		    (settings.records[i].ciboRaccolto < 0) || (settings.records[i].punti < 0))
		{
			danneggiato = TRUE;
		}
		else  {
			for (j=0; j<=30; j++)  {
				// Se non c'Š nemmeno un '\0' in trentuno caratteri
				// allora si Š verificato un errore
				if(settings.records[i].nome[j]=='\0')
					break;
				else if(j==30)
					danneggiato = TRUE;
			}
		}
	}

	if (danneggiato == FALSE)
		return (0);

	cls(color=makecolor(BLUE, WHITE));
	cornice(1, 4, 80, 49, color);

	dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

	dv_printxy(31, 14, "CRITICAL ERROR!", color+BLINK);

	dv_printxy(15, 18, "File di inizializzazione danneggiato!", color);
	dv_printxy(15, 20, "Records danneggiati!", color);
	dv_printxy(15, 23, "Impossibile caricare correttamente i records", color);
	dv_printxy(15, 25, "e i parametri di impostazione del programmma.", color);
	dv_printxy(15, 28, "Si desidera che il file di configurazione sia", color);
	dv_printxy(15, 30, "cancellato automaticamente? (s/n)", color);
	c=getch();
	if (c=='S' || c=='s')  {
		noRdOnlyAttr(cfgFileName);
		if(unlink(cfgFileName)==0)
			dv_printxy(15, 33, "File eliminato.", color);
		else  {
			dv_printxy(15, 33, "Si sono verificati problemi nell'eliminazione", color);
			dv_printxy(15, 35, "del file. Provvedere diversamente.", color);
		}
	}  else  {
		dv_printxy(15, 33, "Provvedere alla riparazione del file", color);
		dv_printxy(15, 35, "o alla sua eliminazione e in seguito riavviare Snake.", color);
		dv_printxy(18, 38, "-->", color);
		dv_printxy(22, 38, cfgFileName, color);
		getch();
		exit(-1);
	}

	while (1)   {
		displayTime();
		c=getch();
		if (c==GC_ENTER || c==GC_ESC)	break;
	}
	return(-1);
}


// saveSettings()	Salva i parametri nel file di configurazione.
//			Viene chiamata alla chiusura del programma.
//			Il file di configurazione pu• esistere gi… o no.
//			In ogni caso Š stato chiuso e la saveSettings()
//			lo riapre cercando in ogni modo di sovrasciverlo
//			per la memorizzazione finale della struct settings.
void saveSettings(void)  {
	FILE *cfg;
	register i=0;

	// Cerca di creare o sovrascrivere il cfgfile per poterci scrivere.
	if((cfg=fopen(cfgFileName, "wb"))==NULL)  {
	// Se non ci riesce forse Š perch‚ Š attivo l'attributo solo-lettura.
	// Quindi chiama questa funzione che lo toglie...
		if (noRdOnlyAttr(cfgFileName))	  // ma in caso di errore...
			return;		// ...niente da fare, ritorno subito.
		// Altrimenti, lo apro nuovamente:
		else		// ma se ancora non mi riesce...
			if((cfg=fopen(cfgFileName, "wb"))==NULL)
				return;	 // ...allora il problema Š un altro...
	}

	// Se invece Š tutto a posto
	// Ci salva i parametri fondamentali...
	fwrite(&settings.livello, sizeof(int), 1, cfg);
	fwrite(&settings.labirinto, sizeof(int), 1, cfg);
	fwrite(&settings.foreGroundColor, sizeof(int), 1, cfg);
	fwrite(&settings.backGroundColor, sizeof(int), 1, cfg);
	// ...e i records...
	for (i=0;i<MAXRECORDS; i++)  {
		fwrite(&(settings.records[i].punti), sizeof(int), 1, cfg);
		fwrite(&(settings.records[i].livello), sizeof(int), 1, cfg);
		fwrite(&(settings.records[i].labirinto), sizeof(int), 1, cfg);
		fwrite(&(settings.records[i].ciboRaccolto), sizeof(int), 1, cfg);
		fwrite(&(settings.records[i].durataGioco), sizeof(unsigned long), 1, cfg);
		fwrite(settings.records[i].nome, sizeof(char), 31, cfg);
	}
	// ...e infine lo chiude.
	fclose(cfg);

	// E attiva l'attr solo-lettura per _evitare_ che possa essere
	// modificato involontariamente dall'esterno...
	setRdOnlyAttr(cfgFileName);
	// ...e l'attributo hidden
	setHiddenAttr(cfgFileName);
}


// displayTime()	Stampa in basso a destra dello schermo l'ora attuale
//			E' utilizzato in attesa di scelta da parte dell'utente
//			nei menu del programma.
void displayTime(void)  {
	struct time ora;
	while(!kbhit())  {
		gettime(&ora);
		dv_printfxy(68, 50, color, "%2d:%02d:%02d", ora.ti_hour, ora.ti_min, ora.ti_sec);
	}
}


void gioca()  {
	char c;
	FILE *fp;
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *gameMenu[] = {"Inizia un nuovo gioco",
				  "Carica l'ultima partita salvata",
				  "Torna indietro"};

	struct Snake snake;


	// Se non c'Š nessuna partita salvata, Š inutile far apparire il menu.
	if ((fp=fopen(savedGame, "r")) == NULL)  {
		playSnake(&snake, STARTNEWGAME);
		return;
	}
	fclose(fp);

	while(1)  {
		cls(color);
		cornice(1, 4, 80, 49, color);

		dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

		dv_printxy(32, 13, "Gioca a Snake", color);

		for (i=0; i<3; i++)
			dv_printxy(15, 24+i*3, gameMenu[i], color);

		do  {
			dv_printxy(15, 24+prev*3, gameMenu[prev], color);
			dv_printxy(15, 24+curr*3, gameMenu[curr], ~color);

			do  {
				displayTime();
				scelta=getch();
			}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
				  scelta!=GC_ESC && scelta!=GC_ENTER);
			prev=curr;
			switch(scelta)  {
				case GC_UP:
					if (curr==0)	curr=2;
					else		curr--;
					break;
				case GC_DOWN:
					if (curr==2)	curr=0;
					else		curr++;
			}
			if (scelta==GC_ESC)	curr=2;
		}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

		switch(curr)  {
			case 0:
				playSnake(&snake, STARTNEWGAME);
				return;
			case 1:
				playSnake(&snake, RELOADGAME);
				// return;
			case 2:
				return;
		}
	}
}


void aggiornaStato(struct Snake *snk)  {
	unsigned long durata = snk->durataGioco;

	dv_printfxy(5, 50, color, "Stato:  Liv: %d - Punti: %d - Mele: %d ",
		snk->livello, snk->punti, snk->ciboRaccolto);

	if (durata < 6000L)
		sprintf(buffer, "%7lu:%02lu", (long)(durata/100), (long)(durata%100));
	else  {
		durata /= 100;
		if (durata < 3600L)
			sprintf(buffer, "%7lu:%02lu", (long)(durata/60),
						    (long)(durata)%60);
		else
			sprintf(buffer, "%4lu:%02lu:%02lu", (long)(durata/3600),
						 (long)((durata%3600)/60),
						 (long)((durata%3600)%60));
	}
	dv_printxy(65, 50, buffer, color);

}


// playNewGame()	Questa Š la funzione centrale di tutto il programmino.
void playSnake(struct Snake *snake, int game)  {
	register i=0, j=0;		// per cicli vari
	char scelta;
	char direzione, keypress;
	struct time ora;
	int flag=0;
	int partitaInCorso = FALSE;
	int velocita[] = {250, 200, 160, 125, 95, 70, 55, 40, 30};
	char *ciboExtra[] = {"û", "ÖÂ·", "Ú×¿", "ø", "", "@"};
	int preso;	// carattere sul quale Š andato a finire il serpente con la testa

	randomize();	// inizializza le funzioni di generazione di numeri casuali


	// Può sembrare strana questa linea di codice, ma basta sapere qualcosa
	// sull'ottimizzazione del codice nelle istruzioni condizionali:
	// se la prima espressione è vera, allora viene eseguita anche la seconda,
	// se invece la prima è falsa, la seconda non viene verificata (perché,
	// siccome c'è un AND, la if() sarebbe stata comunque falsa).
	if ((game==RELOADGAME) && ((flag=caricaPartita(snake))==0))   {
		cls(color);

		// riempio di ' ' lo spazio tra la cornice e il titolo
		for (i=1; i<=80; i++)		// per evitare che possa
			for(j=1; j<4; j++)	// esservi messo del cibo
				dv_putcharxy(i, j, ' ', color);

		dv_printxy(40-strlen(titolo)/2, 2, titolo, color);
		cornice(1, 4, 80, 49, color);
		visualizzaLabirinto(snake->labirinto);

		// stabilisco la direzione verso cui stava andando
		// (questo controllo Š utile ad evitare schianti all'avvio
		// di una partita caricata)
		if(snake->x[snake->snklen] == snake->x[snake->snklen-1])  {
			if(snake->y[snake->snklen] < snake->y[snake->snklen-1])
				direzione = GC_UP;
			else	direzione = GC_DOWN;
		}  else  {
			if(snake->x[snake->snklen] < snake->x[snake->snklen-1])
				direzione = GC_LEFT;
			else	direzione = GC_RIGHT;
		}

		for (i=0; i<snake->snklen; i++)  {
			// Stampo a video il serpentello
			dv_putcharxy(snake->x[i], snake->y[i], 'þ', color);
		}

		// Visualizzo la testa
		dv_putcharxy(snake->x[snake->snklen], snake->y[snake->snklen], '', color);

		// il cibo
		dv_putcharxy(snake->cibox, snake->ciboy, 'ì', color);

		// il ciboExtra
		if (snake->ciboExtraPresente)
			dv_printxy(snake->ciboExtrax, snake->ciboExtray, ciboExtra[snake->tipoCiboExtra], color+BLINK);

		// Tutto Š pronto: serve solo la partecipazione del giocatore
		dv_printxy(10, 50, "Premi un tasto qualsiasi "
				   "per riprendere il gioco", color+BLINK);
	}  else  {
		char c;
		switch(flag)  {
			case 1:		// ha sbagliato password
				return;
			case -1:	// voleva caricare la partita ma si sono verificati errori
				drawWin(23, 20, 57, 30, dialogBoxColor);
				dv_printxy(26, 20, " Errore! ", dialogBoxColor+BLINK);
				dv_printxy(25, 23, "Si sono verificati errori nel", dialogBoxColor);
				dv_printxy(25, 25, "caricamento del gioco.", dialogBoxColor);
				dv_printxy(25, 27, "Impossibile proseguire!", dialogBoxColor);
				getch();
				drawWin(20, 22, 60, 28, dialogBoxColor);
				dv_printxy(22, 25, "Desideri iniziare una nuova partita?", dialogBoxColor+BLINK);
				c=getch();
				if (c!='s' && c!='S')
					return;
				game = STARTNEWGAME;
			case 0:		// vuole iniziare una nuova partita
				setmem(snake, sizeof(*snake), 0x00);
				snake->livello = settings.livello;
				snake->labirinto = settings.labirinto;
		}
	}


	do  {
		// se ancora deve cominciare a giocare, carico tutti i
		// parametri e gli apro il gioco
		if (partitaInCorso==FALSE)  {
			if (game==STARTNEWGAME)  {
				cls(color);

				// riempio di ' ' lo spazio tra la cornice e il titolo
				for (i=1; i<=80; i++)		// per evitare che possa
					for(j=1; j<4; j++)	// esservi messo del cibo
						dv_putcharxy(i, j, ' ', color);

				dv_printxy(40-strlen(titolo)/2, 2, titolo, color);
				cornice(1, 4, 80, 49, color);
				visualizzaLabirinto(snake->labirinto);

				direzione = keypress = GC_RIGHT;

				snake->snklen = MINSNAKELENGTH;
				for (i=0; i<snake->snklen; i++)  {
					// inizializzo il corpo minimo del serpente...
					snake->x[i]=20+i;
					snake->y[i]=26;
					// ...e lo stampo a video
					dv_putcharxy(snake->x[i], snake->y[i], 'þ', color);
				}

				// Imposto le coordinate della testa del serpente...
				snake->testax = snake->x[i] = 20+i;	// testax == x[snklen]
				snake->testay = snake->y[i] = 26;	// testay == y[snklen]
				// ... e la visualizzo
				dv_putcharxy(snake->x[snake->snklen], snake->y[snake->snklen], '', color);

				// Ora mi occupo del cibo:
				do  {
					snake->cibox = random(80)+1;	// trovo le
					snake->ciboy = random(49)+1;	// coodinate...
				}  while(dv_getchar(snake->cibox, snake->ciboy));
				// ...e lo stampo a video
				dv_putcharxy(snake->cibox, snake->ciboy, 'ì', color);

				// Tutto Š pronto: serve solo la partecipazione del giocatore
				dv_printxy(10, 50, "Premi un tasto qualsiasi "
						   "per iniziare a giocare", color+BLINK);
			}
			while (!kbhit());
			for (i=1; i<=80; i++)
				dv_putcharxy(i, 50, ' ', color);
			partitaInCorso = TRUE;
		}
		else  {		//  partitaInCorso == TRUE
			// sta giocando ma ha premuto ESC

			char *gameMenu[] = {"Continua",
					    "Salva il gioco ed esci",
					    "Torna al menu (termina la partita)"};
			char winBuffer[41][11];
			int prev=0, curr=0;

			for(i=0; i<=40; i++)
				for(j=0; j<=10; j++)
					winBuffer[i][j] = dv_getchar(i+20, j+20);

			drawWin(20, 20, 60, 30, dialogBoxColor);

			dv_printxy(23, 20, " Partita in corso... ", dialogBoxColor);

			for (i=0; i<3; i++)
				dv_printxy(22, 23+i*2, gameMenu[i], dialogBoxColor);

			do  {

				dv_printxy(22, 23+prev*2, gameMenu[prev], dialogBoxColor);
				dv_printxy(22, 23+curr*2, gameMenu[curr], ~dialogBoxColor);

				do
					scelta=getch();
				while ( scelta!=GC_DOWN && scelta!=GC_UP &&
					scelta!=GC_ESC && scelta!=GC_ENTER);
				prev=curr;
				switch(scelta)  {
					case GC_UP:
						if (curr==0)	curr=2;
						else		curr--;
						break;
					case GC_DOWN:
						if (curr==2)	curr=0;
						else		curr++;
						break;
					case GC_ESC:
						curr=0;
				}
			}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

			switch(curr)  {
				case 1:
					salvaPartita(snake);
				case 2:		// vuole uscire
					return;
			}

			if (curr==0) {	// gli preparo lo schermo per continuare
				for(i=0; i<=40; i++)
					for(j=0; j<=10; j++)
						dv_putcharxy(i+20, j+20, winBuffer[i][j], color);

				if (snake->ciboExtraPresente==TRUE)
					dv_printxy(snake->ciboExtrax, snake->ciboExtray, ciboExtra[snake->tipoCiboExtra], color+BLINK);

				// Ora tutto Š pronto per ripartire:
				for (i=1; i<=80; i++)	dv_putcharxy(i, 50, ' ', color);
				dv_printxy(10, 50, "Premi un tasto qualsiasi "
						   "per continuare a giocare", color+BLINK);

				while (!kbhit());
				for (i=1; i<=80; i++)	dv_putcharxy(i, 50, ' ', color);
			}
		}

		do  {
			delay(velocita[snake->livello-1]);
			snake->durataGioco += velocita[snake->livello-1] / 10;
			aggiornaStato(snake);

			if(kbhit())  {
				keypress = getch();

				// gli evito di andare nella direzione uguale...
				if(direzione==keypress)  {
					while(kbhit())
						keypress=getch();
				}
				if((keypress!=GC_ESC && keypress!=GC_ENTER &&
				    keypress!=GC_UP && keypress!=GC_DOWN &&
				    keypress!=GC_LEFT && keypress!=GC_RIGHT))
					continue;

				// ...o opposta a quella attuale
				switch(keypress)  {
					case GC_UP:
						if(direzione!=GC_DOWN)
							direzione = keypress;
						break;
					case GC_LEFT:
						if (direzione!=GC_RIGHT)
							direzione = keypress;
						break;
					case GC_DOWN:
						if (direzione!=GC_UP)
							direzione=keypress;
						break;
					case GC_RIGHT:
						if (direzione!=GC_LEFT)
							direzione=keypress;
						break;
					case GC_ENTER:	// PAUSA
						for (i=1; i<48; i++)
							dv_putcharxy(i, 50, ' ', color);
						dv_printxy(10, 50, "PAUSA", color+BLINK);
						getch();
						for (i=1; i<48; i++)
							dv_putcharxy(i, 50, ' ', color);
						break;
					case GC_ESC:
						if (snake->punti == 0)
							return;
						continue;
				}
			}

			switch(direzione)  {
				case GC_UP:
					--snake->testay;	break;
				case GC_RIGHT:
					++snake->testax;	break;
				case GC_DOWN:
					++snake->testay;	break;
				case GC_LEFT:
					--snake->testax;
			}

			// contiene il carattere "colpito" dalla testa del serpente
			preso = dv_getchar(snake->testax, snake->testay);

			// avr… preso la cornice
			if(preso)  {
				if(snake->testax==1)	snake->testax=79;
				if(snake->testax==80)	snake->testax=2;
				if(snake->testay==4)	snake->testay=48;
				if(snake->testay==49)	snake->testay=5;
				preso = dv_getchar(snake->testax, snake->testay);
			}

			// 1. ha sbattuto:
			if (preso=='þ' || preso=='Ä' || preso=='³' ||
			    preso=='À' || preso=='Ù' ||
			    ((snake->ciboExtraPresente==0) && (preso=='Ú' || preso=='Â' || preso=='¿')))  {

				for (i=0; i<=snake->snklen; i++)
					dv_setattr(snake->x[i], snake->y[i], color+BLINK);
				drawWin(25, 20, 55, 32, dialogBoxColor);
				dv_printxy(29, 22, "CRASH!!  Hai sbattuto!", dialogBoxColor);
				dv_printxy(34, 25, "GAME OVER!!!", dialogBoxColor+BLINK);
				sound(500);
				delay(400);
				nosound();
				while(kbhit())	getch();
				sprintf(buffer, "Hai totalizzato %d punti", snake->punti);
				dv_printxy(40-strlen(buffer)/2, 30, buffer, dialogBoxColor+BLINK);
				getch();
				// Inserisco il punteggio in graduatoria, ovviamente
				// se Š abbastanza alto da poter esservi messo...
				if ((snake->punti > settings.records[MAXRECORDS-1].punti) ||
				    ((snake->punti == settings.records[MAXRECORDS-1].punti) &&
				     (snake->durataGioco < settings.records[MAXRECORDS-1].durataGioco)))  {
					record(getRecordAndSave(snake));
					return;		// ritorno al menu principale
				}
				partitaInCorso = FALSE;
				game = STARTNEWGAME;

				setmem(snake, sizeof(*snake), 0x00);
				snake->livello = settings.livello;
				snake->labirinto = settings.labirinto;

				break;
			}
			// 2. ancora Š vivo: quindi...
			else  {
			    // 2.0. ha preso il cibo:
				if(preso=='ì')  {
					snake->ciboRaccolto++;
					do  {
						snake->cibox = random(80)+1;
						snake->ciboy = random(49)+1;
					}  while(dv_getchar(snake->cibox, snake->ciboy));
					dv_putcharxy(snake->cibox, snake->ciboy, 'ì', color);
					dv_putcharxy(snake->x[snake->snklen], snake->y[snake->snklen], 'þ', color);
					snake->snklen++;
					snake->punti += snake->livello;
					aggiornaStato(snake);
					if (snake->snklen>=MAXSNAKELENGTH)  {
					// difficile che ci arrivi qualcuno... ;-)
						// Metto la testa nella nuova coordinata...
						dv_putcharxy(snake->testax, snake->testay, '', color);
						drawWin(14, 20, 66, 31, dialogBoxColor);
						dv_printxy(33, 22, "COMPLIMENTI!!!", dialogBoxColor+BLINK);
						for (i=0; i<5; i++)  {
							sound(i*500);
							delay(100);
							nosound();
						}
						if (snake->livello < 9)  {
							dv_printxy(27, 25, "Hai superato il  ø livello", dialogBoxColor);
							dv_putcharxy(43, 25, '0'+snake->livello, dialogBoxColor);
							dv_printxy(16, 27, "Sei stato promosso subito al livello successivo", dialogBoxColor);
							dv_printxy(26, 29, "con un bonus di 100 punti!!!", dialogBoxColor);
							snake->livello++;
							snake->punti += 100;
							while(kbhit())	getch();
							getch();
						}  else  {
							dv_printxy(27, 25, "Hai completato il 9ø livello", dialogBoxColor);
							dv_printxy(18, 27, "Ti Š stato accreditato un bonus di 300 punti", dialogBoxColor);
							dv_printxy(25, 29, "per il tuo risultato finale!!!", dialogBoxColor);
							snake->punti += 300;
							while(kbhit())	getch();
							getch();
							if ((snake->punti > settings.records[MAXRECORDS-1].punti) ||
							    ((snake->punti == settings.records[MAXRECORDS-1].punti) &&
							     (snake->durataGioco < settings.records[MAXRECORDS-1].durataGioco)))  {
								record(getRecordAndSave(snake));
								return;		// ritorno al menu principale
							}
						}

						setmem(snake->x, MAXSNAKELENGTH*sizeof(int), 0x00);
						setmem(snake->y, MAXSNAKELENGTH*sizeof(int), 0x00);
						snake->ciboExtraPresente = FALSE;

						partitaInCorso = FALSE;
						game = STARTNEWGAME;

						break;
					}
					// gestione animaletti
					if ((snake->ciboRaccolto) && !(snake->ciboRaccolto%5))  {
						int flag = 0, i;
						// ogni 5 mele va stampato un animale (oltre ad una nuova mela)
						snake->tipoCiboExtra = random(6);
						do  {	// determino la posizione del nuovo animaletto
							snake->ciboExtrax = random(80)+1;
							snake->ciboExtray = random(49)+1;
							for (i=0; i<strlen(ciboExtra[snake->tipoCiboExtra]); i++)  {
								if (flag = dv_getchar(snake->ciboExtrax+i, snake->ciboExtray))  {
									break;
								}
							}
						}  while(flag);
						dv_printxy(snake->ciboExtrax, snake->ciboExtray, ciboExtra[snake->tipoCiboExtra], color+BLINK);
						snake->mosse = 60;
						snake->ciboExtraPresente = TRUE;
					}
				}

			    // 2.1. ha preso il ciboExtra
				if (snake->ciboExtraPresente)  {
					if(preso=='' || preso=='û' || preso=='Ö' ||
					   preso=='Â' || preso=='·' || preso=='Ú' ||
					   preso=='×' || preso=='¿' || preso=='ø' ||
					   preso=='' || preso=='@')  {

						// cancello il ciboExtra...
						int i;
						for (i=0; i<strlen(ciboExtra[snake->tipoCiboExtra]); i++)  {
							dv_putcharxy(snake->ciboExtrax+i, snake->ciboExtray, 0x00, color);
						}
						// ...e il contatore delle mosse restanti
						dv_printxy(75, 4, "ÍÍÍÍÍ", color);

						// Metto al posto della testa il 'þ'
						dv_putcharxy(snake->x[snake->snklen], snake->y[snake->snklen], 'þ', color);
						snake->snklen++;
						snake->punti += (((snake->livello-1)*5) +12) + (snake->mosse*2/3);
						snake->mosse=0;
						snake->ciboExtraPresente=FALSE;
					}
				}

			    // 2.2. niente:
				if (snake->ciboExtraPresente)  {	// se c'Š l'animaletto
					snake->mosse--;	// ad ogni mossa della
					// serpe (ad ogni ciclo) diminuisco
					// la var mosse (quasi un timer)
					if (snake->mosse==0) {		// se Š scaduto
					// il tempo, cancello l'animaletto
						int i;
						for (i=0; i<strlen(ciboExtra[snake->tipoCiboExtra]); i++)  {
							dv_putcharxy(snake->ciboExtrax+i, snake->ciboExtray, 0x00, color);
						}
						// e la visualizzazione delle mosse restanti
						dv_printxy(75, 4, "ÍÍÍÍ", color);
						snake->ciboExtraPresente=FALSE;
					}  else  {	// visualizzo le mosse restanti
						dv_printfxy(75, 4, color, "µ%02dÆ", snake->mosse);
					}
				}

				// cancello la testa e metto il 'þ'
				dv_putcharxy(snake->x[snake->snklen], snake->y[snake->snklen], 'þ', color);

				// metto la testa nella nuova coordinata...
				dv_putcharxy(snake->testax, snake->testay, '', color);


				// ...e tolgo l'ultima.
				dv_putcharxy(snake->x[0], snake->y[0], 0x00, color);

				// Aggiorno il serpente
				for(i=0; i<snake->snklen; i++)  {
					snake->x[i] = snake->x[i+1];
					snake->y[i] = snake->y[i+1];
				}
				snake->x[snake->snklen] = snake->testax;
				snake->y[snake->snklen] = snake->testay;
			}
		}  while(keypress!=GC_ESC);

	}  while(1);
}


// getRecordAndSave()	Richiede i dati relativi al nuovo record da salvare,
//			quali il nome (da input) e (come parametri) il
//			punteggio ottenuto e le mele raccolte. Bisogna che
//			sia stato gi… effettuato il controllo per stabilire
//			se il nuovo record sia tale da dover essere effettiva-
//			mente inserito in classifica, ovvero messo tra i primi
//			MAXRECORDS classificati.
//			Inserisce automaticamente il nuovo record nel posto
//			in classifica opportuno, ordinando questa in base
//			innanzitutto ai punti, ma in caso di punteggio uguale
//			valuta la durata del gioco come secondo parametro.
//			Se il nuovo punteggio risulta essere il primo in
//			in classifica ritorna 1.
int getRecordAndSave(struct Snake *snk)  {
	register int i;
	char tmp_nome[31];

	drawWin(14, 20, 66, 32, dialogBoxColor);
	dv_printxy(33, 22, "COMPLIMENTI!!!", dialogBoxColor+BLINK);
	// se Š addirittura il primo, stampo un messaggio diverso...
	if (snk->punti > settings.records[0].punti)
		dv_printxy(25, 25, "Hai stabilito un nuovo record!", dialogBoxColor);
	else	dv_printxy(16, 25, "Il tuo punteggio merita un posto in graduatoria!", dialogBoxColor);
	dv_printxy(30, 27, "Digita il tuo nome:", dialogBoxColor);

	_setcursortype(_SOLIDCURSOR);
	setxy(25, 29);
	do
		getstring(tmp_nome, 30);
	while (!tmp_nome[0]);
	_setcursortype(_NOCURSOR);

	// Ora lo salvo.

	// Lo inserisco subito all'ultimo posto della classifica...
	i=MAXRECORDS-1;
	settings.records[i].punti = snk->punti;
	settings.records[i].livello = snk->livello;
	settings.records[i].labirinto = snk->labirinto;
	settings.records[i].ciboRaccolto = snk->ciboRaccolto;
	settings.records[i].durataGioco = snk->durataGioco;
	strcpy(settings.records[i].nome, tmp_nome);

	for(i--;
		(i>=0) &&
		((settings.records[i+1].punti > settings.records[i].punti) ||
		((settings.records[i+1].punti == settings.records[i].punti) &&
		 (settings.records[i+1].durataGioco < settings.records[i].durataGioco)));
	    i--)  {
	// ... e lentamente (se meritevole :) lo faccio scalare in alto
		snk->punti = settings.records[i].punti;
		snk->livello = settings.records[i].livello;
		snk->labirinto = settings.records[i].labirinto;
		snk->ciboRaccolto = settings.records[i].ciboRaccolto;
		snk->durataGioco = settings.records[i].durataGioco;
		strcpy(tmp_nome, settings.records[i].nome);

		settings.records[i].punti = settings.records[i+1].punti;
		settings.records[i].livello = settings.records[i+1].livello;
		settings.records[i].labirinto = settings.records[i+1].labirinto;
		settings.records[i].ciboRaccolto = settings.records[i+1].ciboRaccolto;
		settings.records[i].durataGioco = settings.records[i+1].durataGioco;
		strcpy(settings.records[i].nome, settings.records[i+1].nome);

		settings.records[i+1].punti = snk->punti;
		settings.records[i+1].livello = snk->livello;
		settings.records[i+1].labirinto = snk->labirinto;
		settings.records[i+1].ciboRaccolto = snk->ciboRaccolto;
		settings.records[i+1].durataGioco = snk->durataGioco;
		strcpy(settings.records[i+1].nome, tmp_nome);
	}
	// A salvarlo nel file di cfg, ci pensa una chiamata a saveSettings()
	// all'uscita del prog.

	return(i == -1);
}


//
int caricaPartita(struct Snake *snake)  {
	char pwd[11] = "", pwd_in[11];
	char *p;
	FILE * gm;
	int flag = 0;

	if((gm=fopen(savedGame, "rb")) == NULL)
		return(-1);

	if (fread(pwd, sizeof(char), 10, gm) != 10)
		return(-1);

	for (p=pwd; *p; p++)
		*p+=43;

	// faccio il controllo sulla password
	cls(color);
	do  {
		drawWin(25, 20, 55, 28, dialogBoxColor);
		sprintf(buffer, "%snserisci la password:", (flag==0 ? "I" : "Rei"));
		dv_printxy(40-strlen(buffer)/2, 23, buffer, dialogBoxColor);
		_setcursortype(_SOLIDCURSOR);
		setxy(35, 25);
		if (getpassword(pwd_in, 10)[0] == '\0')   {
			_setcursortype(_NOCURSOR);
			return(1);
		}
		_setcursortype(_NOCURSOR);
		flag = strcmp(pwd, pwd_in);
		if (flag)  {
			drawWin(27, 20, 53, 28, dialogBoxColor);
			dv_printxy(36, 23, "Errore!", dialogBoxColor+BLINK);
			dv_printxy(32, 25, "Password Errata!", dialogBoxColor);
			getch();
		}
	} while(flag);

	fread(&snake->livello, sizeof(int), 1, gm);
	fread(&snake->labirinto, sizeof(int), 1, gm);
	fread(&snake->snklen, sizeof(int), 1, gm);
	fread(snake->x, sizeof(int), MAXSNAKELENGTH, gm);
	fread(snake->y, sizeof(int), MAXSNAKELENGTH, gm);
	fread(&snake->testax, sizeof(int), 1, gm);
	fread(&snake->testay, sizeof(int), 1, gm);
	fread(&snake->ciboRaccolto, sizeof(int), 1, gm);
	fread(&snake->cibox, sizeof(int), 1, gm);
	fread(&snake->ciboy, sizeof(int), 1, gm);
	fread(&snake->ciboExtrax, sizeof(int), 1, gm);
	fread(&snake->ciboExtray, sizeof(int), 1, gm);
	fread(&snake->tipoCiboExtra, sizeof(int), 1, gm);
	fread(&snake->mosse, sizeof(int), 1, gm);
	fread(&snake->ciboExtraPresente, sizeof(int), 1, gm);
	fread(&snake->punti, sizeof(int), 1, gm);
	fread(&snake->durataGioco, sizeof(unsigned long), 1, gm);

	fclose(gm);
	return(0);
}


//
int salvaPartita(struct Snake *snake)  {
	char pwd[11] = "";
	char *p;
	FILE * gm;
	register i=0;

	if((gm=fopen(savedGame, "wb")) == NULL)
		return(-1);

	cls(color);
	do  {
		drawWin(25, 20, 55, 28, dialogBoxColor);
		sprintf(buffer, "%snserisci la password:", (i==0 ? "I" : "Rei"));
		dv_printxy(40-strlen(buffer)/2, 23, buffer, dialogBoxColor);
		_setcursortype(_SOLIDCURSOR);
		setxy(35, 25);
		while (!getpassword(pwd, 10)[0]);
		_setcursortype(_NOCURSOR);
		for (i=0; i<strlen(pwd); i++)   {
			if (!isalnum(pwd[i]))  {
				drawWin(17, 20, 63, 28, dialogBoxColor);
				dv_printxy(36, 23, "Errore!", dialogBoxColor);
				dv_printxy(19, 25, "Sono ammessi solo caratteri alfanumerici!", dialogBoxColor);
				getch();
				break;
			}
		}
	} while(!isalnum(pwd[i]) && (i!=strlen(pwd)));

	// niente di particolare, ma non Š che devo tenere a bada i cracker!
	for (p=pwd; *p; p++)
		*p-=43;

	fwrite(pwd, sizeof(char), 10, gm);
	fwrite(&snake->livello, sizeof(int), 1, gm);
	fwrite(&snake->labirinto, sizeof(int), 1, gm);
	fwrite(&snake->snklen, sizeof(int), 1, gm);
	fwrite(snake->x, sizeof(int), MAXSNAKELENGTH, gm);
	fwrite(snake->y, sizeof(int), MAXSNAKELENGTH, gm);
	fwrite(&snake->testax, sizeof(int), 1, gm);
	fwrite(&snake->testay, sizeof(int), 1, gm);
	fwrite(&snake->ciboRaccolto, sizeof(int), 1, gm);
	fwrite(&snake->cibox, sizeof(int), 1, gm);
	fwrite(&snake->ciboy, sizeof(int), 1, gm);
	fwrite(&snake->ciboExtrax, sizeof(int), 1, gm);
	fwrite(&snake->ciboExtray, sizeof(int), 1, gm);
	fwrite(&snake->tipoCiboExtra, sizeof(int), 1, gm);
	fwrite(&snake->mosse, sizeof(int), 1, gm);
	fwrite(&snake->ciboExtraPresente, sizeof(int), 1, gm);
	fwrite(&snake->punti, sizeof(int), 1, gm);
	fwrite(&snake->durataGioco, sizeof(unsigned long), 1, gm);

	fclose(gm);
	return(0);
}


void record(int primo) {
	register int i;
	char c;

	if (primo && settings.records[0].nome[0])  {
		cls(color);
		for (i=1; !(c=kbhit()) && i<100; i++)  {
			for(; i%10; i++)
				dv_putcharxy(random(80)+1, random(50)+1, '*', makecolor(settings.backGroundColor, YELLOW));
			clrwin(22, 20, 58, 29, color);
			dv_printxy(31, 22, "Primo classificato:", color);
			dv_printxy(40-strlen(settings.records[0].nome)/2, 25, settings.records[0].nome, color);
			sprintf(buffer, "%d", settings.records[0].punti);
			dv_printxy(40-strlen(buffer)/2, 27, buffer, color);
			sleep(1);
		}

		if (c)  {
			if((c=getch())==GC_ESC)
				return;
			delay(500);
		}
	}

	cls(color);
	cornice(1, 4, 80, 49, color);

	dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

	dv_printxy(31, 9, "Punteggi migliori", color);

	dv_printfxy(6, 13, color, "Posto  %-30s Liv. Lab. Cibo  Durata  Punti", "Nome");

	for (i=0; (i<MAXRECORDS) && (settings.records[i].nome[0]); i++)  {
		dv_printfxy(9, 16+(i*2), color,
		       "%2d  %-30s   %d    %d   %3d   %2d:%02d   %4d",
			i+1,
			     settings.records[i].nome,
				     settings.records[i].livello,
					   settings.records[i].labirinto,
						settings.records[i].ciboRaccolto,
							(int)(settings.records[i].durataGioco/6000),
							(int)((settings.records[i].durataGioco/100)%60),
								 settings.records[i].punti);
	}

	if (settings.records[0].nome[0])
		dv_printxy(12, 50, "Per cancellare tutti i record premere ALT-C", color);

	while (1)   {
		displayTime();
		c=getch();
		switch(c)  {
			case GC_ENTER:
			case GC_ESC:
				return;
			case GC_ALT_C:
				if (settings.records[0].nome[0])  {
					drawWin(16, 22, 64, 28, dialogBoxColor);
					dv_printxy(19, 25, "Vuoi veramente cancellare tutti i record ?", dialogBoxColor);
					c=getch();
					if (c=='s' || c=='S')  {
						for(i=0; i<MAXRECORDS; i++)
							settings.records[i].nome[0] =
							settings.records[i].livello =
							settings.records[i].labirinto =
							settings.records[i].ciboRaccolto =
							settings.records[i].durataGioco =
							settings.records[i].punti = 0;
					}
					record(0);
					return;
				}
		}
	}
}


void menu_OpzioniGioco(void)  {
	char c;
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *optionMenu[] = {"Livello",
				    "Labirinti",
				    "Torna indietro"};

	while(1)  {
		cls(color);
		cornice(1, 4, 80, 49, color);

		dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

		dv_printxy(32, 13, "Opzioni di gioco", color);

		for (i=0; i<3; i++)
			dv_printxy(15, 24+i*3, optionMenu[i], color);

		do  {
			dv_printxy(15, 24+prev*3, optionMenu[prev], color);
			dv_printxy(15, 24+curr*3, optionMenu[curr], ~color);

			do  {
				displayTime();
				scelta=getch();
			}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
				  scelta!=GC_ESC && scelta!=GC_ENTER);
			prev=curr;
			switch(scelta)  {
				case GC_UP:
					if (curr==0)	curr=2;
					else		curr--;
					break;
				case GC_DOWN:
					if (curr==2)	curr=0;
					else		curr++;
			}
			if (scelta==GC_ESC)	curr=2;
		}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

		switch(curr)  {
			case 0:
				subMenu_Livelli();	break;
			case 1:
				subMenu_Labirinti();	break;
			case 2:
				return;
		}
	}
}


void subMenu_Livelli(void)  {
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *levelMenu[]  = {"Livello 1",
				    "Livello 2",
				    "Livello 3",
				    "Livello 4",
				    "Livello 5",
				    "Livello 6",
				    "Livello 7",
				    "Livello 8",
				    "Livello 9",
				    "Torna Indietro"};

	cls(color);
	cornice(1, 4, 80, 49, color);

	dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

	dv_printxy(24, 12, "Menu Livello (livello attuale:  )", color);
	dv_putcharxy(55, 12, (char)(settings.livello+'0'), color);

	for (i=0; i<10; i++)
		dv_printxy(15, 16+i*3, levelMenu[i], color);

	do  {

		dv_printxy(15, 16+prev*3, levelMenu[prev], color);
		dv_printxy(15, 16+curr*3, levelMenu[curr], ~color);

		do  {
			displayTime();
			scelta=getch();
		}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
			  scelta!=GC_ESC && scelta!=GC_ENTER &&
			  (scelta < '1' || scelta > '9'));
		prev=curr;
		switch(scelta)  {
			case GC_UP:
				if (curr==0)	curr=9;
				else		curr--;
				break;
			case GC_DOWN:
				if (curr==9)	curr=0;
				else		curr++;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				curr = scelta - '1';
				scelta =  GC_ENTER;
		}
		if (scelta==GC_ESC)	curr=9;
	}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

	if (curr<9)	settings.livello=curr+1;
}


// labirinti()	Gestisce il menu di scelta, visualizzazione e impostazione
//		di un labirinto
void subMenu_Labirinti(void)  {
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *labirintoMenu[]  = {"Senza labirinto",
					"Scatola",
					"Tunnel",
					"Spirale",
					"Bloccaggio",
					"Contorsione",
					"Torna Indietro"};
	while (1)  {
		cls(color);
		cornice(1, 4, 80, 49, color);

		dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

		dv_printxy(16, 12, "Menu Labirinto - labirinto attuale:", color);
		dv_printxy(52, 12, labirintoMenu[settings.labirinto], color);

		dv_printxy(12, 50, "Per visualizzare il labirinto premere ALT+V", color);

		for (i=0; i<7; i++)
			dv_printxy(15, 16+i*3, labirintoMenu[i], color);

		do  {

			dv_printxy(15, 16+prev*3, labirintoMenu[prev], color);
			dv_printxy(15, 16+curr*3, labirintoMenu[curr], ~color);

			do  {
				displayTime();
				scelta=getch();
			}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
				  scelta!=GC_ESC && scelta!=GC_ENTER && scelta!=GC_ALT_V);
			prev=curr;
			switch(scelta)  {
				case GC_UP:
					if (curr==0)	curr=6;
					else		curr--;
					break;
				case GC_DOWN:
					if (curr==6)	curr=0;
					else		curr++;
					break;
				case GC_ALT_V:
					cls(color);
					cornice(1, 4, 80, 49, color);
					visualizzaLabirinto(curr);
					getch();
					break;
				case GC_ENTER:
					if (curr<6)
						settings.labirinto=curr;
				case GC_ESC:
					return;
			}
		}  while(scelta!=GC_ALT_V);
	}
}


// visualizzaLabirinto()	Come da nome, visualizza il labirinto il cui
//				relativo numero Š passato come parametro.
//				E' utilizzata sia nella semplice visualizzazione
//				del labirinto affinch‚ l'utente lo possa vedere
//				prima di impostarlo, sia nella stampa a video
//				durante il gioco vero e proprio.
void visualizzaLabirinto(int lab)  {
	register i;
	// 0. Senza labirinto
	switch(lab)  {
		case 1:  // 1. Scatola
			dv_putcharxy(2, 5, 'Ú', color);
			for (i=3; i<79; i++)   {
				dv_putcharxy(i, 5, 'Ä', color);
				dv_putcharxy(i, 48, 'Ä', color);
			}
			dv_putcharxy(79, 5, '¿', color);
			for(i=6; i<48; i++)  {
				dv_putcharxy(2, i, '³', color);
				dv_putcharxy(79, i, '³', color);
			}
			dv_putcharxy(2, 48, 'À', color);
			dv_putcharxy(79, 48, 'Ù', color);
			break;
		case 2:	 // 2. Tunnel
			dv_printxy(3, 5, "ÄÄ", color);
			dv_printxy(77, 5, "ÄÄ", color);
			dv_printxy(3, 48, "ÄÄ", color);
			dv_printxy(77, 48, "ÄÄ", color);
			dv_putcharxy(2, 6, '³', color);
			dv_putcharxy(2, 7, '³', color);
			dv_putcharxy(2, 46, '³', color);
			dv_putcharxy(2, 47, '³', color);
			dv_putcharxy(79, 6, '³', color);
			dv_putcharxy(79, 7, '³', color);
			dv_putcharxy(79, 46, '³', color);
			dv_putcharxy(79, 47, '³', color);
			dv_putcharxy(2, 5, 'Ú', color);
			dv_putcharxy(79, 5, '¿', color);
			dv_putcharxy(2, 48, 'À', color);
			dv_putcharxy(79, 48, 'Ù', color);
			dv_printxy(27, 19, "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ", color);
			dv_printxy(27, 33, "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ", color);
			break;
		case 3:	 // 3. Spirale
			for(i=42; i<80; i++)
				dv_putcharxy(i, 19, 'Ä', color);
			for(i=2; i<40; i++)
				dv_putcharxy(i, 33, 'Ä', color);
			for (i=5; i<=26; i++)
				dv_putcharxy(32, i, '³', color);
			for (i=27; i<=48; i++)
				dv_putcharxy(48, i, '³', color);
			break;
		case 4:	 // 4. Bloccaggio
			for (i=3; i<=78; i++)  {
				dv_putcharxy(i, 5, 'Ä', color);
				dv_putcharxy(i, 48, 'Ä', color);
			}
			dv_putcharxy(2, 5, 'Ú', color);
			dv_putcharxy(79, 5, '¿', color);
			dv_putcharxy(2, 48, 'À', color);
			dv_putcharxy(79, 48, 'Ù', color);
			for(i=0; i<=3; i++)  {
				dv_putcharxy(2, 6+i, '³', color);
				dv_putcharxy(2, 47-i, '³', color);
				dv_putcharxy(79, 6+i, '³', color);
				dv_putcharxy(79, 47-i, '³', color);
			}
			for (i=0; i<=12; i++)   {
				dv_putcharxy(2, 10+i, '³', color);
				dv_putcharxy(79, 10+i, '³', color);
				dv_putcharxy(23, 10+i, '³', color);
				dv_putcharxy(56, 10+i, '³', color);
				dv_putcharxy(2, 43-i, '³', color);
				dv_putcharxy(79, 43-i, '³', color);
				dv_putcharxy(23, 43-i, '³', color);
				dv_putcharxy(56, 43-i, '³', color);
			}
			for (i=0; i<=25; i++)   {
				dv_putcharxy(27+i, 16, 'Ä', color);
				dv_putcharxy(27+i, 37, 'Ä', color);
			}
			break;
		case 5:	 // 5. Contorsione
			for(i=2; i<=79; i++)
				dv_putcharxy(i, 33, 'Ä', color);
			for(i=2; i<39; i++)
				dv_putcharxy(i, 20, 'Ä', color);
			for(i=45; i<=79; i++)
				dv_putcharxy(i, 20, 'Ä', color);
			for(i=34; i<=48; i++)
				dv_putcharxy(42, i, '³', color);
			for(i=6; i<=19; i++)
				dv_putcharxy(39, i, '³', color);
			dv_putcharxy(39, 20, 'Ù', color);
			dv_putcharxy(2, 5, 'Ú', color);
			dv_putcharxy(2, 6, '³', color);
			dv_putcharxy(3, 5, 'Ä', color);
			dv_putcharxy(4, 5, 'Ä', color);
			for(i=15; i<=70; i++)
				dv_putcharxy(i, 5, 'Ä', color);
			dv_putcharxy(39, 5, 'Â', color);
			dv_putcharxy(42, 33, 'Â', color);
	}
}


void menu_OpzioniProg(void) {
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *optionMenu[]  = {"Imposta colore del testo",
				     "Imposta colore di sfondo",
				     "Torna indietro"};

	while(1)  {
		cls(color=makecolor(settings.backGroundColor, settings.foreGroundColor));
		cornice(1, 4, 80, 49, color);

		dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

		dv_printxy(31, 13, "Opzioni programma", color);

		for (i=0; i<3; i++)
			dv_printxy(15, 24+i*3, optionMenu[i], color);

		do  {
			dv_printxy(15, 24+prev*3, optionMenu[prev], color);
			dv_printxy(15, 24+curr*3, optionMenu[curr], ~color);

			do  {
				displayTime();
				scelta=getch();
			}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
				  scelta!=GC_ESC && scelta!=GC_ENTER);
			prev=curr;
			switch(scelta)  {
				case GC_UP:
					if (curr==0)	curr=2;
					else		curr--;
					break;
				case GC_DOWN:
					if (curr==2)	curr=0;
					else		curr++;
			}
			if (scelta==GC_ESC)	curr=2;
		}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

		switch(curr)  {
			case 0:
				subMenu_ForeGcolor();	break;
			case 1:
				subMenu_BackGcolor();	break;
			case 2:
				return;
		}
	}
}


// backGcolor()		Gestisce il menu di impostazione del colore di sfondo
void subMenu_BackGcolor(void)  {
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *backGcolorMenu[]  = {"Nero",
					 "Blu",
					 "Verde",
					 "Azzurro",
					 "Rosso",
					 "Magenta",
					 "Marrone",
					 "Grigio Chiaro",
					 "Torna indietro"};

	cls(color);
	cornice(1, 4, 80, 49, color);

	dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

	dv_printxy(24, 12, "Menu Imposta colore di sfondo", color);

	for (i=0; i<9; i++)
		dv_printxy(15, 15+i*3, backGcolorMenu[i], color);

	do  {

		dv_printxy(15, 15+prev*3, backGcolorMenu[prev], color);
		dv_printxy(15, 15+curr*3, backGcolorMenu[curr], ~color);

		do  {
			displayTime();
			scelta=getch();
		}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
			  scelta!=GC_ESC && scelta!=GC_ENTER);
		prev=curr;
		switch(scelta)  {
			case GC_UP:
				if (curr==0)	curr=8;
				else		curr--;
				break;
			case GC_DOWN:
				if (curr==8)	curr=0;
				else		curr++;
		}
		if (scelta==GC_ESC)	curr=8;
	}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

	if (curr<8)	settings.backGroundColor = curr;
}


// foreGcolor()		Gestisce il menu di impostazione del colore del testo
void subMenu_ForeGcolor(void)  {
	register i=0;
	int curr=0, prev=0;
	char scelta;
	const char *foreGcolorMenu[] = {"Nero",
		 "Blu",			"Verde",
		 "Azzurro",		"Rosso",
		 "Magenta",		"Marrone",
		 "Grigio chiaro",	"Grigio scuro",
		 "Blu chiaro",		"Verde chiaro",
		 "Azzurro chiaro",	"Rosso Chiaro",
		 "Magenta chiaro",	"Giallo",
		 "Bianco",		"Torna Indietro"};

	cls(color);
	cornice(1, 4, 80, 49, color);

	dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

	dv_printxy(24, 10, "Menu Imposta colore del testo", color);

	for (i=0; i<17; i++)
		dv_printxy(15, 13+i*2, foreGcolorMenu[i], color);

	do  {

		dv_printxy(15, 13+prev*2, foreGcolorMenu[prev], color);
		dv_printxy(15, 13+curr*2, foreGcolorMenu[curr], ~color);

		do  {
			displayTime();
			scelta=getch();
		}  while (scelta!=GC_DOWN && scelta!=GC_UP &&
			  scelta!=GC_ESC && scelta!=GC_ENTER);
		prev=curr;
		switch(scelta)  {
			case GC_UP:
				if (curr==0)	curr=16;
				else		curr--;
				break;
			case GC_DOWN:
				if (curr==16)	curr=0;
				else		curr++;
		}
		if (scelta==GC_ESC)	curr=16;
	}  while(scelta!=GC_ENTER && scelta!=GC_ESC);

	if (curr<16)	settings.foreGroundColor = curr;
}


// informazioni()	Niente di particolare da segnalare.
void informazioni()  {
	char c;
	cls(color);
	cornice(1, 4, 80, 49, color);

	dv_printxy(40-strlen(titolo)/2, 2, titolo, color);

	dv_printxy(30, 14, "Informazioni su Snake", color);

	dv_printxy(15, 20, "Sviluppato da SnakePlissken (Antonio Macr)", color);
	dv_printxy(15, 22, "E-mail:  -snake-plissken-@libero.it", color);
	dv_printfxy(15, 26, color, "Compiled on %s at %s", __DATE__, __TIME__);

	dv_printxy(15, 35, "Se noti qualche bug, te ne sarei grato se vorrai", color);
	dv_printxy(15, 36, "riportarmelo all'indirizzo e-mail sovrastante.", color);

	dv_printfxy(15, 30, color, "Linee di codice:  %d di C puro :)", __LINE__);

	while (1)   {
		displayTime();
		c=getch();
		if (c==GC_ENTER || c==GC_ESC)	break;
	}
}