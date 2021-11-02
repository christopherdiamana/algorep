#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include "mpi.h"

#define TIMEOUT 400
#define INTIAL_HEARTBEAT 75
#define MIN_ELECTION_TIMEOUT 500
#define MAX_ELECTION_TIMEOUT 1000


class Server
{

    public:
        enum class Status { Follower, Candidate, Leader };
        /*** CONSTRUCTOR ***/
        Server(int rank, int numberOfNodes);

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
        int numberOfNodes;
        //
        Status state;
        // Current Epoch
        int currentTerm;
        // The timeout for an epoch
        int timeout;
        // The timeout for the hearbeat
        int heartbeatTimeout;
        // The timeout for election
        int electionTimeout;

        // Négatif signifie aucun leader; Sinon spécifie le rang du leader.
        int leaderRank;

        //
        clock_t lastTimer;

        int votedFor;
        // The index of log entry
        int commitIndex;
        // RAM log storage
        std::string currentLog;
        // Storage emplacement in the hard drive.
        std::ofstream log;
        // The epochs count
        unsigned long term;

        /*** METHODS ***/

        void startElection();
        void toCandidate();
        void toLeader();
        void toFollower();
        void heartbeat();
        bool requestVote();
        std::ofstream setLog();
        void setStatus(int statusIndex);



};
