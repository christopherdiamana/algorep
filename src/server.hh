#pragma once

#include <iostream>
#include <fstream>

#define MIN_TIMEOUT 100
#define MAX_TIMEOUT 400
#define INTIAL_HARTBEAT 50


class Server
{

    public:
        enum class Status { Follower, Candidate, Leader };
        /*** CONSTRUCTOR ***/
        Server(unsigned long rank, unsigned long size);

        /*** METHODS ***/
        bool vote(unsigned long serverRank);
        Status getStatus();
        int getLeader();
        void setTimeout();


    private:
        /*** ATTRIBUTES  ***/

        unsigned long rank;
        // Size of the system
        unsigned long size;
        Status state;
        // Current Epoch
        int currentTerm;
        // The timeout for an epoch
        int timeout;
        // The timeout for the hearbeat
        int heartbeatTimeout;

        // 0 signifie aucun leader; Sinon spécifie le rang du leader.
        unsigned long leaderRank;

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
        void setLog();
        void setStatus(int statusIndex);



};
