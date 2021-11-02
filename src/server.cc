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

//Sending Heartbeat function for the leader !!!DO NOT USE WITH A FOLLOWER!
void Server::sendHeartbeat()
{
  //redundant but just in case you decide to ignore the previous comment.
  if (state == Status::Leader)
  {
    //Send an heartbeat to every Node except hisself
    for (int i = 0; i < Server::numberOfNodes; i++)
    {
      if (i == rank)
      {
        continue;
      }
      char *text = "HeartBeat";
      MPI_Send(text, sizeof(text)/sizeof(char), MPI_BYTE, i, -1 * term, MPI_COMM_WORLD);
    }
  }
}

void Server::receiveMessage()
{
  //Initialise Status
  MPI_Status tmpStatus;
  //Probe allows to fake receive the message and fill the status with the source and the tag
  MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &tmpStatus);
  //MPI_Get_Count allows us to know the len of the message
  int numberAmount = 0;
  MPI_Get_count(&tmpStatus, MPI_BYTE, &numberAmount);
  //Create buffer to store in-coming data
  char buffer[numberAmount];
  //Create new Status (pretty sure overwriting the probe one would be good aswell)
  MPI_Status realStatus;
  //Receive the message who was scouted
  MPI_Recv(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, tmpStatus.MPI_SOURCE, tmpStatus.MPI_TAG, MPI_COMM_WORLD, &realStatus);
  if (realStatus.MPI_TAG < 0)
  {
    if (term <= realStatus.MPI_TAG * -1)
    {
      state = Status::Follower;
      leaderRank = realStatus.MPI_SOURCE;
      //Dead code: Should followers answer to the leader? #TODO DISCUSS ME
      //char *text = "HeartBeat";
      //MPI_Send(text, sizeof(text)/sizeof(char), MPI_BYTE, leaderRank, -1, MPI_COMM_WORLD);
    }
  }
  else if (realStatus.MPI_TAG > 0) //FIXME if TAG happens to be 0 then a term++ is missing in the code;
  {
    //Vote demande from a server.
    if (strncmp(buffer, "v_", 2) == 0 && realStatus.MPI_SOURCE < numberOfNodes)
    {

    }
    //Demande Client originale ou relayée.
    else if (realStatus.MPI_SOURCE >= numberOfNodes || strncmp(buffer, "r_", 2) == 0)
    {
      if (strncmp(buffer, "v_", 2) != 0)
      {
        char newBuffer[strlen(buffer) + 3] = "v_";
        strcat(newBuffer, buffer);
        //Use newBuffer to send Msg
        Server::handleRequest(newBuffer, realStatus.MPI_TAG);
      }
      else
      {
        Server::handleRequest(buffer, realStatus.MPI_TAG);
      }
    }
    //Reception d'une demande d'append Entry par le leader.
    if (strncmp(buffer, "A_", 2) == 0 && realStatus.MPI_SOURCE == leaderRank)
    {
      //Remove the "A_" tag and write it on the disc
      char* logText = buffer + 2;
      Server::log << logText << std::endl;
      term++;
    }
  }

}

void Server::handleRequest(char* buffer, int tag)
{
  //1 Est ce déjà en Ram si oui => 4 sinon => 2

  //2 Mettre en RAM => 3

  //3 Envoyez à tous les nodes et préparer un tableau de retour sinon
  if (state == Status::Leader)
  {
    for (int i = 0; i < Server::numberOfNodes; i++)
    {
      if (i == rank)
      {
        continue;
      }

      MPI_Send(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, i, term, MPI_COMM_WORLD);
    }
  }
  //préparer un tableau de retour

  //4 Remplir le tableau de retour jusqu'à majorité.

  //Si majorité atteinte envoi d'append Entry plus écriture sur disque SINON SI pas de changement de leader mise en place du rollback
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
    else if (state == Status::Leader)
    {
      if (clock() - this->lastTimer > this->timeout)
      {
        sendHeartbeat();
      }
    }
    else
    {

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
