#include "server.hh"


Server::Server(unsigned long rank, unsigned long size)
  : rank(rank)
    , size(size)
    , status(Folower)
    , currentTerm(0)
    , timeout()
    , heartbeatTimeout(50)
    , leaderRank()
    , votedFor()
    , commitIndex()
    , currentLog()
    , log()
{
    setTimeout();
    LOG(DEBUG) << "server " << rank_ << "has PID " << getpid();
}

void Server::setTimeout(){
  this->timeout = std::rand() % (MAX_TIMEOUT - MIN_TIMEOUT) + MIN_TIMEOUT;
  this->heartbeatTimeout = INTIAL_HARTBEAT
}

 void Server::setLog(){
   filename = "server_" + std::to_string(rank) + ".log";
   this->log.open(filename);
 }

