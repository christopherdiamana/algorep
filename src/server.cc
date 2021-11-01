#include "server.hh"

#include "message.hh"


Server::Server(unsigned long rank, unsigned long size)
  : rank(rank)
    , size(size)
    , state(Status::Follower)
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
    setLog();
    LOG(DEBUG) << "server " << rank << "has PID " << getpid();
}

void Server::setTimeout(){
  this->timeout = std::rand() % (MAX_TIMEOUT - MIN_TIMEOUT) + MIN_TIMEOUT;
  this->heartbeatTimeout = INTIAL_HARTBEAT
}

void Server::setLog(){
  filename = "bin/server_" + std::to_string(rank) + ".log";
  this->log.open(filename);
}

void Server::start(){
  if (this->state == Status::Follower)
    this->toCandidate()
}


void Server::toCandidate(){
  // Update attributes
  this->state = Status::Candidate;
  this->votedFor = this->rank;
  this->currentTerm += 1;

  int voteCounter = 1;

  // Vote request
  requestVote()
}
