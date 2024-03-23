#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sqlite3.h>
#include <signal.h>

#define TIMP 120

// codul de eroare returnat de anumite apeluri
extern int errno;

// portul de conectare la server
int port;
int j = 1, sd;
char msgrasp[100];

void handle_sigint1(int nr_sem)
{
}

void handle_sigint2(int nr_sem)
{
  strcpy(msgrasp, "Deconectat ");
  if (write(sd, msgrasp, 100) <= 0)
  {
    perror("[client]Eroare la write() catre server(1).\n");
    exit(0);
  }

  close(sd);
  exit(0);
}

int callback(void *data, int argc, char **argv, char **colNames)
{
  for (int i = 0; i < argc; i++)
  {
    printf("Locul %d: ", j);
    printf("Participantul:%s ", argv[i]);
    i++;
    printf("Punctajul:%s\n", argv[i]);
    j++;
  }
  return 0;
}

int main(int argc, char *argv[])
{
  int k = 0, ok = 0;
  struct sockaddr_in server;
  char rez[100], verif_msg[10], message[1000];
  time_t start_time, current_time;
  double elapsed_time;
  signal(SIGINT, handle_sigint1);

  if (argc != 3)
  {
    printf("[client]Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }

  port = atoi(argv[2]);

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[client]Eroare la socket().\n");
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons(port);

  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[client]Eroare la connect().\n");
    return errno;
  }

  char msg[100000];
  while (1)
  {
    int bytes = read(sd, msg, sizeof(msg));

    if (bytes <= 0)
    {
      printf("[client]Eroare la read() de la server(1).\n");
      close(sd);
    }

    printf("[client]Mesaj de la server: %s\n", msg);

    int i;
    for (i = 0; i <= 6; i++)
      verif_msg[i] = msg[i];
    verif_msg[i] = '\0';

    k++;
    if (k == 3 || strcmp(verif_msg, "Eroare!") == 0)
      break;

    memset(msg, 0, strlen(msg));
  }

  signal(SIGINT, handle_sigint2);

  if (strcmp(verif_msg, "Eroare!") != 0)
  {
    bzero(msgrasp, 100);
    printf("[client]Introduceti un fisier cu rezolvarea:\n");

    time(&start_time);
    do
    {
      time(&current_time);
      elapsed_time = difftime(current_time, start_time);

      if (elapsed_time >= TIMP)
      {
        break;
      }

      fd_set actfds;
      FD_ZERO(&actfds);
      FD_SET(STDIN_FILENO, &actfds);

      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 0;

      int r = select(STDIN_FILENO + 1, &actfds, NULL, NULL, &tv);

      if (r > 0 && FD_ISSET(STDIN_FILENO, &actfds))
      {
        fflush(stdout);
        read(0, msgrasp, 100);
        ok = 1;
        break;
      }

      usleep(10000);
    } while (1);

    if (ok == 0)
    {
      printf("[client]S-a terminat timpul si problema nu a fost trimisa!\n");
      strcpy(msgrasp, " ");
    }
    if (write(sd, msgrasp, 100) <= 0)
    {
      perror("[client]Eroare la write() catre server(2).\n");
      return errno;
    }

    if (read(sd, message, 1000) < 0)
    {
      perror("[client]Eroare la read() de la server(2).\n");
      return errno;
    }
    printf("\n[client]Mesaj de la server: %s\n\n", message);

    printf("[client]Asteptam rezultatele...\n");

    if (read(sd, rez, 100) < 0)
    {
      perror("[client]Eroare la read() de la server(3).\n");
      return errno;
    }
    printf("[client]S-au primit rezultatele...\n");
    printf("[client]Mesaj de la server: %s\n\n", rez);
    printf("[client]Si rezultatele sunt:\n");

    char nume[100];
    int i,j=1;
    for(i=0;i<=10;i++)
    {
      nume[i]=rez[j++];
    }
    nume[i]='\0';

    sqlite3 *db;
    char *errMsg = 0, comm[100];

    int rc = sqlite3_open(nume, &db); 

    if (rc != SQLITE_OK)
    {
      fprintf(stderr, "[client]Nu se poate deschide baza de date: %s\n", sqlite3_errmsg(db));
      return rc;
    }

    strcpy(comm, "SELECT * FROM rezolvari ORDER BY punctaj DESC;");
    const char *selectDataSQL = comm;
    int rez_pb = sqlite3_exec(db, selectDataSQL, callback, 0, &errMsg);

    if (rez_pb != SQLITE_OK)
    {
      fprintf(stderr, "[client]Eroare la SQL: %s\n", errMsg);
      sqlite3_free(errMsg);
    }

    sqlite3_close(db);
  }

  // inchidem conexiunea, am terminat
  close(sd);
}
