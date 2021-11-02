#pragma once

#include <iostream>
#include <fstream>
#include "mpi.h"

#define MIN_TIMEOUT 100
#define MAX_TIMEOUT 400
#define INTIAL_HARTBEAT 50


class Server
{

    public:
        enum class Status { Follower, Candidate, Leader };
        /*** CONSTRUCTOR ***/
        Server(int rank, int size);

        /*** DESTRUCTOR ***/
        ~Server();

        /*** METHODS ***/
        bool vote(unsigned long serverRank);
        Status getStatus();
        int getLeader();
        void setTimeout();
        bool update();


    private:
        /*** ATTRIBUTES  ***/
        int rank;
        // Size of the system can't be negative
        int size;
        Status state;
        // Current Epoch
        int currentTerm;
        // The timeout for an epoch
        int timeout;
        // The timeout for the hearbeat
        int heartbeatTimeout;

        // Négatif signifie aucun leader; Sinon spécifie le rang du leader.
        int leaderRank;

        int votedFor;
        // The index of log entry
        int commitIndex;
        // RAM log storage
        std::string currentLog;
        // Storage emplacement in the hard drive.
        std::ofstream log;

        /*** METHODS ***/

        void start();
        void toCandidate();
        void toLeader();
        void toFollower();
        void heartbeat();
        bool requestVote();
        std::ofstream setLog();
        void setStatus(int statusIndex);



};
