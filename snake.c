#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#include <time.h>
#include <sys/select.h>

#define INIT_ROW	10
#define INIT_COL	10
#define NUMAR_OBSTACOLE		20
#define LUNGIME_SARPE	1000
#define SELECT_EVENT	1

typedef struct {
	int x, y;
}snake;


int main(void)
{
	int row = INIT_ROW, col = INIT_COL, new_row, new_col;
	int nrows, ncols;
	int hranax, hranay;	// coordonatele hranei
	int i = 5, k = 0, FOREVER = 1, verificare, speedup = 0, timp = 200000, light, scor = 0; // k - lungimea sarpelui, i - o variabila oarecare (e initializata cu 5 pentru ca am folosit-o initial pentru a crea sarpele pe ecran), verificare - variabila folosita in cazul in care se genereaza hrana unde nu trebuie (spre exemplu peste un obstacol, peste sarpe sau chenar), speedup - folosit pentru a mari viteza sarpelui la fiecare 2 patratele de hrana mancate, timp - timpul in milisecunde pentru functia select, light - folosit in acelasi scop ca variabila "verificare", am avut nevoie de doua variabile pentru a sti unde sa nu generez hrana

	char c, tasta = 'd'; // tasta - variabila care imi retine ultima tasta apasata pentru a sti cum sa ma folosesc de functia select, e initializata cu 'd' pentru ca am vrut ca sarpele sa se indrepte spre dreapta de cum porneste jocul

	snake sarpe[LUNGIME_SARPE], obstacole[NUMAR_OBSTACOLE]; // nume sugestive


	int nfds, sel;
	fd_set read_descriptors;
	struct timeval timeout;


	/* Se inițializează ecranul; initscr se va folosi în mod obligatoriu */
	WINDOW *wnd = initscr();

	/* getmaxyx - este un macro, nu o funcție, așă că nu avem adresă la parametri */
	/* Se va reține în nrows și ncols numărul maxim de linii și coloane */
	getmaxyx(wnd, nrows, ncols);

	/* Se șterge ecranul */
	clear();
	/* Se inhibă afișarea caracterelor introduse de la tastatură */
	noecho();
	/* Caracterele introduse sunt citite imediat - fără 'buffering' */
	cbreak();

	/* Se ascunde cursorul */	
	curs_set(0);



	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 0;
	timeout.tv_usec = timp;

	start_color(); // schimbare culoare ecran
	init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(1, COLOR_GREEN, COLOR_GREEN);



	/* Se va afișa un mesaj la poziția formată din primii doi parametri - (par1, par2) */	
	mvaddstr(0, 0, "Puteti sa mutati sarpele folosind tastele:");
	mvaddstr(1, 2, "A - stanga");
	mvaddstr(2, 2, "D - dreapta");
	mvaddstr(3, 2, "W - sus");
	mvaddstr(4, 2, "S - jos");
	mvaddstr(5, 0, "Cu obstacole sau fara? Apasati Y pentru da sau N pentru nu:");
	refresh();

	c = getchar(); // am creat 2 if-uri pentru 2 cazuri: daca nu vreau obstacole imi va intra pe primul, daca vreau imi intra pe al doilea

	if (tolower(c) == 'n') {
		clear();

		attron(COLOR_PAIR(2));


		while (i > 0) {
			/* Se mută cursorul la poziția (row, col - i + 1) */
			move(row, col - i + 1);
			sarpe[k].x = row;
			sarpe[k].y = col - i + 1;
			k++;
			/* Se adaugă la poziția indicată de cursor caracterul '*' */
			addch('*');
			i--;
		}

		attron(COLOR_PAIR(1));


		for (i = 0; i < ncols; i++)
			mvaddstr(0, i, "#");
		for (i = 0; i < ncols; i++)
			mvaddstr(nrows - 1, i, "#");
		for (i = 0; i < nrows; i++)
			mvaddstr(i, 0, "#");
		for (i = 0; i < nrows; i++)
			mvaddstr(i, ncols - 1, "#");

		attroff(COLOR_PAIR(1));

		mvprintw(0,ncols/2 - 5,"SCOR:%d", scor);


		// tot ce urmeaza pana la urmatorul comentariu este folosit pentru a genera hrana. Am gandit asa: generez un patratel de hrana, daca hrana nu se afla pe chenar, atunci verific daca se afla pe sarpe, daca nu se afla nici pe sarpe atunci pun patratelul de hrana in locul respectiv, daca nu, voi genera alte coordonate pentru hrana. (la fel si pentru al doilea if, doar ca mai am o conditie in plus ca patratelul sa nu fie generat pe obstacole)

		/* Se pune un patratel cu hrana intr-un loc la intamplare folosit srand */
		srand(time(NULL));

		light = 1;
		verificare = 1;


		while (verificare) {
			hranax = rand() % nrows;
			hranay = rand() % ncols;
			if (hranax != 0 && hranax != (nrows - 1) && hranay != 0 && hranay != (ncols - 1)) {
				for (i = 0; i < k; i++)
					if (sarpe[i].x == hranax && sarpe[i].y == hranay)
						light = 0;
				if (light == 1) {
					verificare = 0;
					attron(COLOR_PAIR(2));
					mvaddstr(hranax, hranay, "*");
					attroff(COLOR_PAIR(2));
					refresh();
				}
			}
		}

		/* Rămânem în while până când se primește tasta q, este atinsa o margine sau sarpele se loveste pe el insusi */
		while (FOREVER) {
			sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
			// daca se apasa vreo tasta inseamna ca sarpele se misca unde i-am spus, daca nu se apasa atunci va executa al doilea bloc de instructiuni si anume, va merge in directia indicata de tasta precedenta
			if (sel == SELECT_EVENT) {
				c = getchar();

				if (tolower(c) == 'q')
					break;

				switch (tolower(c)) {
					case 'a':

						new_row = row;
						if (col > 1)
							new_col = col - 1;
						else FOREVER = 0;
						tasta = 'a';
						break;


					case 'd':

						new_row = row;
						if (col + 1 < ncols - 1)
							new_col = col + 1;
						else FOREVER = 0;
						tasta = 'd';
						break;


					case 'w':

						new_col = col;
						if (row > 1)
							new_row = row - 1;
						else FOREVER = 0;
						tasta = 'w';
						break;


					case 's':

						new_col = col;
						if (row + 1 < nrows - 1)
							new_row = row + 1;
						else FOREVER = 0;
						tasta = 's';
						break;

				}

				// verific ca sarpele sa nu se manance singur, daca o face GAME OVER
				for (i = 0; i < k - 1; i++)
					if (sarpe[i].x == new_row && sarpe[i].y == new_col)
						FOREVER = 0;
				// daca inghite patratelul de hrana contorul speedup va creste cu o unitate, iar patratelul va face parte din vectorul in care memorez fiecare unitate a sarpelui
				if (hranax == new_row && hranay == new_col) {
					scor++;
					speedup++;
					sarpe[k].x = hranax;
					sarpe[k].y = hranay;
					k++;
					light = 1;
					verificare = 1;
					while (verificare) {
						hranax = rand() % nrows;
						hranay = rand() % ncols;
						if (hranax != 0 && hranax != (nrows - 1) && hranay != 0 && hranay != (ncols - 1)) {
							for (i = 0; i < k; i++)
								if (sarpe[i].x == hranax && sarpe[i].y == hranay)
									light = 0;
							if (light == 1) {
								mvprintw(0,ncols/2 - 5,"SCOR:%d", scor);
								verificare = 0;
								attron(COLOR_PAIR(2));
								mvaddstr(hranax, hranay, "*");
								attroff(COLOR_PAIR(2));
								refresh();
							}
						}
					}
				}
				// daca nu a inghitit patratelul, il misc pur si simplu pe ecran prin stergerea ultimului caracter din sarpe si adaugarea lui in directia in care merge, pe post de cap.
				else {
					move(sarpe[0].x, sarpe[0].y);
					addch(' ');
					attron(COLOR_PAIR(2));
					move(new_row, new_col);
					addch('*');
					attroff(COLOR_PAIR(2));
					refresh();
					for (i = 0; i < k -1; i++) {
						sarpe[i].x = sarpe[i+1].x;
						sarpe[i].y = sarpe[i+1].y;
					}
					sarpe[k-1].x = new_row;
					sarpe[k-1].y = new_col;
				}

				row = new_row;
				col = new_col;
			}

			// daca nu s-a selectat nimic de la tastatura, sarpele se muta automat pe ecran (conditiile sunt aceleasi ca si mai sus cu foarte mici modificari)
			else {


				if (tasta == 'a') {
					new_row = row;
					new_col = col - 1;
				}
				else if (tasta == 'd') {
					new_row = row;
					new_col = col + 1;
				}
				else if (tasta == 'w') {
					new_row = row - 1;
					new_col = col;
				}
				else if (tasta == 's') {
					new_row = row + 1;
					new_col = col;
				}

				for (i = 0; i < k - 1; i++)
					if (sarpe[i].x == new_row && sarpe[i].y == new_col)
						FOREVER = 0;

				if (new_col < 1 || new_col > (ncols - 2) || new_row < 1 || new_row > (nrows - 2))
					FOREVER = 0;			

				if (hranax == new_row && hranay == new_col) {
					scor++;
					speedup++;
					sarpe[k].x = hranax;
					sarpe[k].y = hranay;
					k++;
					light = 1;
					verificare = 1;
					while (verificare) {
						hranax = rand() % nrows;
						hranay = rand() % ncols;
						if (hranax != 0 && hranax != (nrows - 1) && hranay != 0 && hranay != (ncols - 1)) {
							for (i = 0; i < k; i++)
								if (sarpe[i].x == hranax && sarpe[i].y == hranay)
									light = 0;
							if (light == 1) {
								mvprintw(0,ncols/2 - 5,"SCOR:%d", scor);
								verificare = 0;
								attron(COLOR_PAIR(2));
								mvaddstr(hranax, hranay, "*");
								attroff(COLOR_PAIR(2));
								refresh();
							}
						}
					}
				}
				else {
					move(sarpe[0].x, sarpe[0].y);
					addch(' ');
					attron(COLOR_PAIR(2));
					move(new_row, new_col);
					addch('*');
					attroff(COLOR_PAIR(2));
					refresh();
					for (i = 0; i < k -1; i++) {
						sarpe[i].x = sarpe[i+1].x;
						sarpe[i].y = sarpe[i+1].y;
					}
					sarpe[k-1].x = new_row;
					sarpe[k-1].y = new_col;
				}
			}

			row = new_row;
			col = new_col;
			FD_SET(0, &read_descriptors);
			timeout.tv_sec = 0;
			// daca speedup-ul ajunge sa fie 2 inseamna ca s-au consumat 2 bucatele de hrana, asadar viteza sarpelui creste, prin urmare timeout.tv_usec scade
			if (speedup % 2 == 0 && speedup != 0) {
				speedup = 0;
				timp = timp - 2500;
				timeout.tv_usec = timp;
			}
			else timeout.tv_usec = timp;

		} 
	}

	// pentru cazul in care vrem si obstacole se va executa urmatorul bloc de instructiuni cu conditiile de rigoare. Sunt aceleasi ca mai sus + conditiile pentru obstacole, unde sunt nume sugestive si nu trebuie explicat nimic
	else if (tolower(c) == 'y') {
		clear();

		attron(COLOR_PAIR(2));

		while (i > 0) {
			/* Se mută cursorul la poziția (row, col - i + 1) */
			move(row, col - i + 1);
			sarpe[k].x = row;
			sarpe[k].y = col - i + 1;
			k++;
			/* Se adaugă la poziția indicată de cursor caracterul '*' */
			addch('*');
			i--;
		}

		attron(COLOR_PAIR(1));

		for (i = 0; i < ncols; i++)
			mvaddstr(0, i, "#");
		for (i = 0; i < ncols; i++)
			mvaddstr(nrows - 1, i, "#");
		for (i = 0; i < nrows; i++)
			mvaddstr(i, 0, "#");
		for (i = 0; i < nrows; i++)
			mvaddstr(i, ncols - 1, "#");



		for(i = 0; i < NUMAR_OBSTACOLE; i++) {
			obstacole[i].x = rand() % nrows;
			obstacole[i].y = rand() % ncols;
			mvaddstr(obstacole[i].x, obstacole[i].y, "#");
		}
		attroff(COLOR_PAIR(1));

		mvprintw(0,ncols/2 - 5,"SCOR:%d", scor);

		/* Se pune un patratel cu hrana intr-un loc la intamplare folosit srand */

		srand(time(NULL));

		light = 1;
		verificare = 1;

		while (verificare) {
			hranax = rand() % nrows;
			hranay = rand() % ncols;
			if (hranax != 0 && hranax != (nrows - 1) && hranay != 0 && hranay != (ncols - 1)) {
				for (i = 0; i < k; i++)
					if (sarpe[i].x == hranax && sarpe[i].y == hranay)
						light = 0;
				for (i = 0; i < NUMAR_OBSTACOLE; i++)
					if (obstacole[i].x == hranax && obstacole[i].y == hranay)
						light = 0;
				if (light == 1) {
					verificare = 0;
					attron(COLOR_PAIR(2));
					mvaddstr(hranax, hranay, "*");
					attroff(COLOR_PAIR(2));
					refresh();
				}
			}
		}

		/* Rămânem în while până când se primește tasta q, este atinsa o margine sau sarpele se loveste pe el insusi */
		while (FOREVER) {
			sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
			if (sel == SELECT_EVENT) {
				c = getchar();

				if (tolower(c) == 'q')
					break;

				/* Se determină noua poziție, în funcție de tasta apăsată
				 * Nu putem depași nrows și ncols, sau linia 0/coloana 0.
				 */

				switch (tolower(c)) {
					case 'a':
						new_row = row;
						if (col > 1)
							new_col = col - 1;
						else FOREVER = 0;
						tasta = 'a';
						break;

					case 'd':
						new_row = row;
						if (col + 1 < ncols - 1)
							new_col = col + 1;
						else FOREVER = 0;
						tasta = 'd';
						break;

					case 'w':
						new_col = col;
						if (row > 1)
							new_row = row - 1;
						else FOREVER = 0;
						tasta = 'w';
						break;

					case 's':
						new_col = col;
						if (row + 1 < nrows - 1)
							new_row = row + 1;
						else FOREVER = 0;
						tasta = 's';
						break;
				}

				for (i = 0; i < k - 1; i++)
					if (sarpe[i].x == new_row && sarpe[i].y == new_col)
						FOREVER = 0;

				for (i = 0; i < NUMAR_OBSTACOLE; i++)
					if (obstacole[i].x == new_row && obstacole[i].y == new_col)
						FOREVER = 0;


				if (hranax == new_row && hranay == new_col) {
					scor++;
					speedup++;
					sarpe[k].x = hranax;
					sarpe[k].y = hranay;
					k++;
					light = 1;
					verificare = 1;
					while (verificare) {
						hranax = rand() % nrows;
						hranay = rand() % ncols;
						if (hranax != 0 && hranax != (nrows - 1) && hranay != 0 && hranay != (ncols - 1)) {
							for (i = 0; i < k; i++)
								if (sarpe[i].x == hranax && sarpe[i].y == hranay)
									light = 0;
							for (i = 0; i < NUMAR_OBSTACOLE; i++)
								if (obstacole[i].x == hranax && obstacole[i].y == hranay)
									light = 0;
							if (light == 1) {
								mvprintw(0,ncols/2 - 5,"SCOR:%d", scor);
								verificare = 0;
								attron(COLOR_PAIR(2));
								mvaddstr(hranax, hranay, "*");
								attroff(COLOR_PAIR(2));
								refresh();
							}
						}
					}
				}
				else {
					move(sarpe[0].x, sarpe[0].y);
					addch(' ');
					attron(COLOR_PAIR(2));
					move(new_row, new_col);
					addch('*');
					attroff(COLOR_PAIR(2));
					refresh();
					for (i = 0; i < k -1; i++) {
						sarpe[i].x = sarpe[i+1].x;
						sarpe[i].y = sarpe[i+1].y;
					}
					sarpe[k-1].x = new_row;
					sarpe[k-1].y = new_col;
				}

				row = new_row;
				col = new_col;
			}
			else {

				if (tasta == 'a') {
					new_row = row;
					new_col = col - 1;
				}
				else if (tasta == 'd') {
					new_row = row;
					new_col = col + 1;
				}
				else if (tasta == 'w') {
					new_row = row - 1;
					new_col = col;
				}
				else if (tasta == 's') {
					new_row = row + 1;
					new_col = col;
				}

				for (i = 0; i < k - 1; i++)
					if (sarpe[i].x == new_row && sarpe[i].y == new_col)
						FOREVER = 0;

				if (new_col < 1 || new_col > (ncols - 2) || new_row < 1 || new_row > (nrows - 2))
					FOREVER = 0;

				for (i = 0; i < NUMAR_OBSTACOLE; i++)
					if (obstacole[i].x == new_row && obstacole[i].y == new_col)
						FOREVER = 0;			

				if (hranax == new_row && hranay == new_col) {
					scor++;
					speedup++;
					sarpe[k].x = hranax;
					sarpe[k].y = hranay;
					k++;
					light = 1;
					verificare = 1;
					while (verificare) {
						hranax = rand() % nrows;
						hranay = rand() % ncols;
						if (hranax != 0 && hranax != (nrows - 1) && hranay != 0 && hranay != (ncols - 1)) {
							for (i = 0; i < k; i++)
								if (sarpe[i].x == hranax && sarpe[i].y == hranay)
									light = 0;
							for (i = 0; i < NUMAR_OBSTACOLE; i++)
								if (obstacole[i].x == hranax && obstacole[i].y == hranay)
									light = 0;
							if (light == 1) {
								mvprintw(0,ncols/2 - 5,"SCOR:%d", scor);
								verificare = 0;
								attron(COLOR_PAIR(2));
								mvaddstr(hranax, hranay, "*");
								attroff(COLOR_PAIR(2));
								refresh();
							}
						}
					}
				}
				else {
					move(sarpe[0].x, sarpe[0].y);
					addch(' ');
					attron(COLOR_PAIR(2));
					move(new_row, new_col);
					addch('*');
					attroff(COLOR_PAIR(2));
					refresh();
					for (i = 0; i < k -1; i++) {
						sarpe[i].x = sarpe[i+1].x;
						sarpe[i].y = sarpe[i+1].y;
					}
					sarpe[k-1].x = new_row;
					sarpe[k-1].y = new_col;
				}
			}

			row = new_row;
			col = new_col;
			FD_SET(0, &read_descriptors);
			timeout.tv_sec = 0;
			if (speedup % 2 == 0 && speedup != 0) {
				speedup = 0;
				timp = timp - 2500;
				timeout.tv_usec = timp;
			}
			else timeout.tv_usec = timp;
		} 
	}

	clear();
	mvprintw(nrows/2 - 3, ncols/4 - 6, "Ai obtinut %d puncte! Apasa orice tasta pentru a iesi din joc.", scor);
	refresh();
	getchar();


	/* Se închide fereastra ncurses */	
	endwin();

	return 0;
}
