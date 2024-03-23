#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <bits/waitflags.h>
#include <sqlite3.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

// portul folosit
#define PORT 2024
#define MAX_CLI 3

int num_clients = 0, client_fds[MAX_CLI], num_prob[100] = {0};
char comm_ins[100];
sqlite3 *db;
char *errMsg = 0;

// codul de eroare returnat de anumite apeluri
extern int errno;

char prob[10], input[10], output[10], timp[10];
// Funcția de callback pentru afișarea rezultatelor interogării SELECT
int callback(void *data, int argc, char **argv, char **colNames)
{
	for (int i = 0; i < argc; i++)
	{
		strcpy(prob, argv[0]);
		strcpy(input, argv[1]);
		strcpy(output, argv[2]);
		strcpy(timp, argv[3]);
	}
	return 0;
}

int sendInfo(int fd) // aceasta functie trimite clientilor toate informatiile dupa ce toti acestia s-au conectat
{
	int nr;
	char comm[100], num[10], msg[100000];
	;

	char messageForAll[] = "Toți participantii s-au conectat. Poate incepe olimpiada!";
	if (write(client_fds[fd], messageForAll, strlen(messageForAll)) < 0)
	{
		perror("[server] Eroare la write() catre client(2).\n");
		return 0;
	}
	nr = 1 + (rand() % 10);
	num_prob[client_fds[fd]] = nr;

	strcpy(comm, "SELECT problema,input,output,timp_min FROM probleme WHERE id = ");
	sprintf(num, "%d", nr);
	strcat(comm, num);
	strcat(comm, ";");
	const char *selectDataSQL = comm;
	int pb = sqlite3_exec(db, selectDataSQL, callback, 0, &errMsg);

	if (pb != SQLITE_OK)
	{
		fprintf(stderr, "Eroare la SQL: %s\n", errMsg);
		sqlite3_free(errMsg);
	}

	bzero(msg, 100);

	strcat(msg, "Problema:");
	strcat(msg, prob);
	strcat(msg, " Input:");
	strcat(msg, input);
	strcat(msg, " Output:");
	strcat(msg, output);
	strcat(msg, " Timp:");
	strcat(msg, timp);
	strcat(msg, "\n\n");
	strcat(msg, "Cerinta este:\n");

	FILE *FD;
	char line[1024];
	char path[100];
	strcpy(path, "Cerinte_prob/");
	strcat(path, prob);
	strcat(path, ".txt");
	FD = fopen(path, "r");
	while (fgets(line, 1024, FD) != NULL)
	{
		strcat(msg, line);
	}
	strcat(msg, "\n\n");
	path[0] = '\0';

	strcat(msg, "Exemplu Input:\n");
	strcat(path, "Cerinte_prob/");
	strcat(path, input);
	FD = fopen(path, "r");
	while (fgets(line, 1024, FD) != NULL)
	{
		strcat(msg, line);
	}
	strcat(msg, "\n\n");
	path[0] = '\0';

	strcat(msg, "Exemplu Output:\n");
	strcat(path, "Cerinte_prob/");
	strcat(path, output);
	FD = fopen(path, "r");
	while (fgets(line, 1024, FD) != NULL)
	{
		strcat(msg, line);
	}
	strcat(msg, "\n\n");

	strcat(msg, "Informatii:\n");
	strcat(msg, "Timpul alocat fiecarei probleme este de 120 min.\n");
	strcat(msg, "Fisierul sursa pe care o sa il trimiteti trebuie sa fie compilabil si rulabil, de asemenea trebuie sa aiba urmatorul format: probN.cpp (N - numarul problemei primite).\n");
	strcat(msg, "Numarul maxim de participanti este 5.\n\n");

	if (write(client_fds[fd], msg, strlen(msg)) < 0)
	{
		perror("[server] Eroare la write() catre client(3)).\n");
		return 0;
	}

	return 1;
}

int recvProb(int fd) // functia care primeste fisierele de la clienti si le raspunde
{
	char msgrasp[100] = " ";

	int bytes = read(fd, msgrasp, 100);
	if (bytes <= 0)
	{
		perror("[server]Eroare la read() de la client.\n");
		return 0;
	}
	msgrasp[strlen(msgrasp) - 1] = '\0';
	char verif_prob[100], num_pb[2];
	strcpy(verif_prob, "prob");
	sprintf(num_pb, "%d", num_prob[fd]);
	strcat(verif_prob, num_pb);
	strcat(verif_prob, ".cpp");

	if (strcmp(verif_prob, msgrasp) == 0)
	{
		printf("[server]Fisierul primit este %s de la participantul cu descriptorul %d.\n", msgrasp, fd);

		char message[] = "Am primit fisierul, de acum se va face evaluarea acestuia, va vom trimite rezultatele la final. Multumim!";
		if (bytes && write(fd, message, strlen(message)) < 0)
		{
			perror("[server] Eroare la write() catre client(7).\n");
			return 0;
		}

		printf("[server]Rezolvarea primita de la acest participant este:\n\n");
		FILE *fd1;
		char line[1024], path[100];

		strcpy(path, "Cerinte_prob/");
		strcat(path, msgrasp);
		fd1 = fopen(path, "r");
		while (fgets(line, 1024, fd1) != NULL)
		{
			printf("%s", line);
		}
		printf("\n\n");

		fclose(fd1);
	}
	else if (strcmp(msgrasp, "Deconectat") == 0)
	{
		printf("[server]Eroare! Participantul cu descriptorul %d s-a deconectat, punctajul va fii 0!\n\n", fd);
		num_prob[fd] = -1;
	}
	else
	{
		printf("[server]Eroare! Fisierul primit de la participantul cu descriptorul %d nu are formatul indicat sau participantul nu a trimis problema in intervalul de timp indicat, punctajul va fii 0!\n\n", fd);
		char message[] = "Fisier incorect sau timpul s-a terminat, ne pare rau! Multumim!";
		if (bytes && write(fd, message, strlen(message)) < 0)
		{
			perror("[server] Eroare la write() catre client(8).\n");
			return 0;
		}
		num_prob[fd] = -1;
	}

	return bytes;
}

void corectare(int fd) // functia in care se realizeaza corectarea problemelor
{
	char num_pb[2];
	sprintf(num_pb, "%d", num_prob[client_fds[fd]]);

	char rezolvare[100];
	strcpy(rezolvare, "./exec.sh "); // un script care compileaza si ruleaza fiecare problema
	strcat(rezolvare, "prob");
	strcat(rezolvare, num_pb);

	int fd2;
	fd2 = open("corectare.txt", O_WRONLY);
	int saved_stdout = dup(1); // salvez stdout-ul curent pentru a-l restabilii mai tarziu
	dup2(fd2, 1);
	system(rezolvare);
	dup2(saved_stdout, 1);
	close(saved_stdout);
	close(fd2);

	FILE *fd3;
	fd3 = fopen("corectare.txt", "r");
	char c[1024];
	fgets(c, 1024, fd3);
	fclose(fd3);

	if (c[0] == 'y')
	{
		printf("[server]Fisierul 'prob%s.cpp' s-a compilat si rulat cu succes!\n", num_pb);
		FILE *fd4, *fd5;
		char line1[1024], line2[1024], path1[100], path2[100], n[10];
		int nr_lini1 = 0, nr_lini2 = 0, punctaj;
		float imp;

		strcpy(path1, "Cerinte_prob/");
		strcat(path1, "prob");
		strcat(path1, num_pb);
		strcat(path1, ".cpp");

		fd4 = fopen(path1, "r");
		while (fgets(line1, 1024, fd4) != NULL)
		{
			nr_lini1++;
		}

		strcpy(path2, "Cerinte_prob/");
		strcat(path2, "RezProb");
		strcat(path2, num_pb);
		strcat(path2, ".cpp");

		fd5 = fopen(path2, "r");
		while (fgets(line2, 1024, fd5) != NULL)
		{
			nr_lini2++;
		}
		if (nr_lini1 >= nr_lini2)
			punctaj = 100;
		else
		{
			imp = (float)nr_lini1 / nr_lini2;
			punctaj = imp * 100;
		}
		sprintf(n, "%d", punctaj);
		strcat(comm_ins, ",");
		strcat(comm_ins, n);
		strcat(comm_ins, ");");
	}
	else
	{
		printf("[server]Fisierul 'prob%s.cpp' nu s-a compilat cu succes, astfel punctajul va fii 0!\n", num_pb);
		strcat(comm_ins, ",0);");
	}
}

int main()
{
	struct sockaddr_in server;
	struct sockaddr_in from;
	int sd, client, nfds, k = 0, OK = 0;
	time_t t;
	srand((unsigned)time(&t));
	char rez[100], comm[100];
	fd_set readfds, actfds;
	struct timeval tv;

	int rc = sqlite3_open("probleme.db", &db); //"probleme.db" este o baza de date care contine problemele cu fisierele de input, output al acestora

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "[server]Nu se poate deschide baza de date: %s\n", sqlite3_errmsg(db));
		return rc;
	}

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server]Eroare la socket().\n");
		return errno;
	}

	bzero(&server, sizeof(server));
	bzero(&from, sizeof(from));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("[server]Eroare la bind().\n");
		return errno;
	}

	if (listen(sd, MAX_CLI) == -1)
	{
		perror("[server]Eroare la listen().\n");
		return errno;
	}

	printf("[server]Asteptam conectarea participantilor la portul %d...\n", PORT);
	fflush(stdout);

	for (int i = 0; i < MAX_CLI; i++)
	{
		client_fds[i] = 0;
	}

	FD_ZERO(&actfds);
	FD_SET(sd, &actfds);

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	nfds = sd;

	strcpy(comm, "DELETE FROM rezolvari;");
	const char *selectDataSQL = comm;
	int rez_pb = sqlite3_exec(db, selectDataSQL, 0, 0, &errMsg);

	if (rez_pb != SQLITE_OK)
	{
		fprintf(stderr, "[server]Eroare la SQL: %s\n", errMsg);
		sqlite3_free(errMsg);
	}

	// servim in mod concurent clientii...
	while (1)
	{
		bcopy((char *)&actfds, (char *)&readfds, sizeof(readfds));

		// Utilizarea funcției select
		int sel = select(nfds + 1, &readfds, NULL, NULL, &tv);

		if (sel < 0)
		{
			perror("[server]Eroare la select()");
			return errno;
		}

		// Verificarea pentru noi conexiuni
		if (FD_ISSET(sd, &readfds))
		{
			socklen_t length = sizeof(from);

			client = accept(sd, (struct sockaddr *)&from, &length);

			if (client < 0)
			{
				perror("[server]Eroare la accept().\n");
				continue;
			}

			if (num_clients < MAX_CLI)
			{

				if (nfds < client)
				{
					nfds = client;
				}

				FD_SET(client, &actfds);

				client_fds[num_clients++] = client;
				printf("[server]'Participantul' cu descriptorul %d s-a conectat si este al %d-lea participant!\n", client, num_clients);

				// Trimite mesaj de bun venit către client
				char message[] = "Bine ati venit la olimpiada de informatica 2023-2024 (on-line). Asteptam conectarea tuturor participantilor!";
				if (write(client, message, strlen(message)) < 0)
				{
					perror("[server] Eroare la write() catre client(1).\n");
					return 0;
				}

				// Verificare dacă toți clienții s-au conectat
				if (num_clients == MAX_CLI)
				{
					printf("[server]Toți participantii s-au conectat. Sa incepem.\n");
					printf("[server]Trimitem informatiile catre participanti\n");
					// Trimite mesaj la fiecare client
					for (int fd = 0; fd < MAX_CLI; fd++)
					{
						if (client_fds[fd] != -1)
						{
							sendInfo(fd);
						}
					}
				}
			}
			else
			{
				char message[] = "Eroare! S-au conectat deja numarul maxim de participanti! Va asteptam la o alta editie a olimpiadei! Multumim!";
				if (write(client, message, strlen(message)) < 0)
				{
					perror("[server] Eroare la write() catre client(5).\n");
					return 0;
				}
				close(client);
			}
		}
		if (num_clients == MAX_CLI && OK == 0)
		{
			printf("[server]Asteptam fisierele...\n\n");
			OK = 1;
		}
		for (int fd = 0; fd <= nfds; fd++) // parcurgem multimea de descriptori
		{
			// este un socket de citire pregatit?
			if (fd != sd && FD_ISSET(fd, &readfds))
			{
				if (recvProb(fd))
				{
					FD_CLR(fd, &actfds);
					k++;
				}
			}
		} // for

		if (k == MAX_CLI) // aici se face corectarea si se  trimit rezultatele la toti participantii
		{
			printf("[server]Am primit toate fisierele, acum se vor compila si rula problemele...Si la final o sa se trimita rezultatele...\n\n");

			for (int fd = 0; fd < MAX_CLI; fd++) // se corecteaza toate problemele
			{
				char part[10];
				strcpy(comm_ins, "INSERT INTO rezolvari VALUES(");
				sprintf(part, "%d", fd + 1);
				strcat(comm_ins, part);
				if (num_prob[client_fds[fd]] != -1)
					corectare(fd);
				else
					strcat(comm_ins, ",0);");

				const char *selectDataSQL = comm_ins;
				int rez_pb = sqlite3_exec(db, selectDataSQL, 0, 0, &errMsg);

				if (rez_pb != SQLITE_OK)
				{
					fprintf(stderr, "[server]Eroare la SQL: %s\n", errMsg);
					sqlite3_free(errMsg);
				}

				memset(comm_ins, 0, strlen(comm_ins));
			}

			for (int fd = 0; fd < MAX_CLI; fd++) // se trimit rezultatele
			{
				strcpy(rez, "'probleme.db' este baza de date in care se afla rezultatele, in tabela 'rezolvari'...");
				if (write(client_fds[fd], rez, 100) < 0)
				{
					perror("[server] Eroare la write() catre client(6).\n");
					return 0;
				}
				close(client_fds[fd]);
			}
			printf("\n[server]S-a terminat olimpiada!\n\n");
			k = 0;
			OK = 0;
			num_clients = 0;

			printf("\n[server]Poate sa inceapa alta olimpiada!\n");
			printf("[server]Asteptam conectarea participantilor la portul %d...\n", PORT);
		}

	} // while

	// inchiderea conexiuni la baza de date SQLite
	sqlite3_close(db);

} // main