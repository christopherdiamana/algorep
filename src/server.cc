#include "server.hh"
#include "message.hh"

#include <unistd.h>
#include <string.h>


Server::Server(int rank, int size)
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
    , log(Server::setLog())
{
    setTimeout();
    log << "server " << rank << "has PID " << getpid();
}

Server::~Server()
{
  log.close();
}

void Server::setTimeout(){
  this->timeout = std::rand() % (MAX_TIMEOUT - MIN_TIMEOUT) + MIN_TIMEOUT;
  this->heartbeatTimeout = INTIAL_HARTBEAT;
}

std::ofstream Server::setLog(){
  const std::string filename = "Logs/server_" + std::to_string(rank) + ".txt";
  std::ofstream log(filename.c_str(), std::ofstream::out | std::ofstream::app);
  return log;
}

void Server::start(){
  if (this->state == Status::Follower)
    this->toCandidate();
}


void Server::toCandidate(){
  // Update attributes
  this->state = Status::Candidate;
  this->votedFor = this->rank;
  this->currentTerm += 1;

  int voteCounter = 1;

  // Vote request
  requestVote();
}

bool update()
{
  //TODO
  return 0;
}
