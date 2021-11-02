#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include "mpi.h"

#define TIMEOUT 400
#define INITIAL_HEARTBEAT 70
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
        //Last Term in which the log got written on the disc.
        int lastWrittenTerm;
        // The timeout for an epoch
        int timeout;
        // The timeout for the hearbeat
        int heartbeatTimeout;
        // The timeout for election
        int electionTimeout;

        // Négatif signifie aucun leader; Sinon spécifie le rang du leader.
        int leaderRank;

        //Chronometer to check timeouts
        clock_t lastTimer;

        int votedFor;
        // The index of log entry
        int commitIndex;
        // RAM log storage
        const char currentLog[256][256];
        // Storage emplacement in the hard drive.
        std::ofstream log;
        // The epochs count
        unsigned long term;
        // Store OnGoing ElectionResults, 0 = Not Answered yet, 1 = Vote no, 2 = Vote yes
        char electionResults[];
        // Store majority proportion to trigger AppendEntry. 0 = Not Answered yet, 1 = Vote no, 2 = Vote yes
        char majorityResults[];



        /*** METHODS ***/

        void startElection();
        void toCandidate();
        void toLeader();
        void toFollower();
        void sendHeartbeat();
        bool requestVote();
        std::ofstream setLog();
        void setStatus(int statusIndex);
        void receiveMessage();
        void Server::handleRequest(char* buffer, int tag);



};
