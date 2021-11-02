#include "server.hh"
#include "message.hh"

#include <unistd.h>
#include <string.h>


Server::Server(int rank, int size)
  : rank(rank)
    , numberOfNodes(size)
    , state(Status::Follower)
    , currentTerm(0)
    , timeout(TIMEOUT)
    , heartbeatTimeout(INTIAL_HEARTBEAT)
    , leaderRank()
    , votedFor()
    , commitIndex()
    , currentLog()
    , lastTimer(clock())
    , log(Server::setLog())
    , term(0)
{
    log << "server " << rank << "has PID " << getpid();
}

Server::~Server()
{
  log.close();
}

void Server::setTimeout(){
  this->heartbeatTimeout = INTIAL_HEARTBEAT;
}

std::ofstream Server::setLog(){
  const std::string filename = "Logs/server_" + std::to_string(rank) + ".txt";
  std::ofstream log(filename.c_str(), std::ofstream::out | std::ofstream::app);
  return log;
}

void Server::startElection(){
  if (this->state == Status::Follower)
  {
    this->term++;
    this->state = Status::Candidate;
    this->electionTimeout = std::rand() % (MAX_ELECTION_TIMEOUT - MIN_ELECTION_TIMEOUT) + MIN_ELECTION_TIMEOUT;
    bool leaderExists = false;
    int foundRank = -1;
    while ( lastTimer + electionTimeout > clock() && !leaderExists)
    {
      /*** if (receiverOwnerId)
       *   {
       *     leaderExists = true;
       *     foundRank = leaderRank;
       *   }
       *   if (winsElection)
       *   {
       *     leaderExists = true;
       *     foundRank = rank;
       *   }
       ***/

    }
    //Election Timeout Scenario
    if (!leaderExists)
    {
      startElection();
    }
    else
    {
      this->leaderRank = foundRank;
      this->state = (leaderRank != rank) ? Status::Follower : Status::Leader;
    }
  }

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

bool Server::update()
{
  //FIXME
  while(true)
  {
    if (clock() - this->lastTimer > this->timeout)
    {
      leaderRank = -1;
      if (state  == Status::Follower)
      {
        startElection();
      }
      else
      {

      }
    }
      //TODO
      /***
       *  if (Receive Client Info)
       *  {
       *     if (leaderRank >= 0)
       *     {
       *       SendToLeader
       *     }
       *     else
       *     {
       *       SendToRandomServer and hope for it to be the new supreme leader
       *     }
       *  }
       *
       ***/
  }
  return 0;
}
