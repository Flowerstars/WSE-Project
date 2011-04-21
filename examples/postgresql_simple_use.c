#include <stdio.h>
#include <stdlib.h>
#include <postgresql/libpq-fe.h>// connect to database


#define STR_CONNECT_DB_INFO "hostaddr='127.0.0.1' port='5432' dbname='wse-proj' user='postgres' password=''"

int main (int argc, char *argv[])
{
    PGconn     *conn;
    PGresult   *PGres;
    conn = PQconnectdb(STR_CONNECT_DB_INFO);
    
    if (PQstatus(conn) != CONNECTION_OK)
    {
        printf("Connect to Postgre ERROR!!! \n");
        return 0;
    }
    else
    {
        printf("Connect to Postgre OK!!\n");
    }

    PGres = PQexec(conn, "select dish_name from dish_info where url_id=2;");

    if (PQresultStatus(PGres) != PGRES_COMMAND_OK && PQresultStatus(PGres) != PGRES_TUPLES_OK)
    {
        printf("command failed!\n");
        PQclear(PGres);
        PQfinish(conn);
        return 0;
    }

    printf("The value I read is  %s\n", PQgetvalue(PGres, 0, 0));
    PQclear(PGres);

    PQfinish(conn);

    printf("DONE!\n");
    return 1;
}


// to compile using: gcc -o postgresql_sample postgresql_simple_use.c -lpq
