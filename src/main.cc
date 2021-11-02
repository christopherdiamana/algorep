#include <iostream>

#include "mpi.h"
#include "server.hh"
#include "client.hh"

int throwArgumentError(char* progName)
{
    std::cerr << "Please do: ./" << progName << " numberOfNode(>0) numberOfClient(>0)" << std::endl;
    return 1;
}



int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        return throwArgumentError(argv[0]);
    }

    int numberOfNode = std::stoi(argv[1]);
    int numberOfClient = std::stoi(argv[2]);

    if (numberOfNode <= 0 || numberOfClient <= 0)
    {
        return throwArgumentError(argv[0]);
    }

    //Initialise MPI
    MPI_Init(&argc, &argv);

    //Retrieve rank and size
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Split processes and avoid incoherences
    if (rank >= size)
    {
        //Si ce code s'execute alors je comprends plus rien.
        std::cerr<<"Rank can't be greater than size!"<<std::endl;
        return 1;
    }


    std::cout<<rank;
    /*if (rank == 0)
    {
        std::cout<<"Insérez entrée utilisateur:"<<std::endl;
        char* test = "";
        std::cin>> test;
        std::cout<<"You entered: "<< test <<std::edl;
    }*/
    if (rank < numberOfNode)
    {
        Server curServer = Server::Server(rank, size);
        //TODO \o/ curServer do something = ^     D
        while (curServer.update())
        {}
    }
    if (rank >= numberOfNode)
    {
        Client curClient = Client::Client(rank, size);
        //TODO curClient ¯\_(ツ)_/¯ ??
    }


    //ShutDown
    MPI_Finalise();
    //Exit main successfully
    return 0;
}