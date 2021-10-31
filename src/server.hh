#pragma once



class Server
{
    public:

        //Champs
        unsigned long rank;
        unsigned long size;

        enum Status {Folower, Candidate, Leader};

        // 0 signifie aucun leader; Sinon spécifie le rang du leader.
        unsigned long leaderRank = 0;

        //Constructeur
        Server(unsigned long rank, unsigned long size) : rank(rank), size(size), status(Folower) {}

        //Méthodes
        bool vote(unsigned long rank);

        Status getStatus();

    private:

        //Champs
        Status status;


        //Méthodes
        void heartbeat();

        bool requestVote();

        void setStatus(int statusIndex);


};