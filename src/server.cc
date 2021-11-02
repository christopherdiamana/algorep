#include "server.hh"
#include "message.hh"

#include <unistd.h>
#include <string.h>
#include <cstdlib>

Server::Server(int rank, int size)
  : rank(rank)
    , numberOfNodes(size)
    , state(Status::Follower)
    , currentTerm(0)
    , timeout(TIMEOUT)
    , heartbeatTimeout(INITIAL_HEARTBEAT)
    , leaderRank()
    , votedFor()
    , commitIndex()
    , currentLog()
    , lastTimer(clock())
    , log(Server::setLog())
    , term(0)
    , waiting(false)
{
    log << "server " << rank << "has PID " << getpid();
    electionResults[size];
    for (int i; i < size; i++)
    {
     *electionResults[i] = 0;
    }

    *majorityResults[size];
    for (int i = 0; i < size; i++)
    {
      *majorityResults[i] = 0;
    }
}

Server::~Server()
{
  log.close();
  //if currentLog is allocated we might need to free curretLog on destruction.
}

void Server::setTimeout(){
  this->heartbeatTimeout = INITIAL_HEARTBEAT;
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
    int leaderRank = -1;

    char *text = "v_voteMe";
    for (int i = 0; i < Server::numberOfNodes; i++)
    {
      if (i == rank)
      {
        continue;
      }
      MPI_Send(text, sizeof(text)/sizeof(char), MPI_BYTE, i, term, MPI_COMM_WORLD);
    }

    lastTimer = clock();
    while ( lastTimer + electionTimeout > clock() && !leaderExists)
    {
      if (leaderRank != -1)
      {
        leaderExists = true;
      }

      receiveMessage();

      int yesBallots = 0;
      int noBallots = 0;
      for (int i = 0; i < numberOfNodes;i++)
      {
        yesBallots += (*electionResults[i] == 2) ? 1 : 0;
        noBallots += (*electionResults[i] == 1) ? 1 : 0;
        if (yesBallots > numberOfNodes / 2)
        {
          leaderRank = rank;
          leaderExists = true;
          break;
        }
        if (noBallots > numberOfNodes / 2)
        {
          break;
        }
      }
    }
    //Election Timeout Scenario
    if (!leaderExists)
    {
      startElection();
    }
    else
    {
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
    lastTimer = clock();
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
  //Initialise Flag
  int flag = 0;
  //Initialise Status
  MPI_Status tmpStatus;
  //Probe allows to fake receive the message and fill the status with the source and the tag
  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag,&tmpStatus);
  if (flag == 0)
  {
    return;
  }
  //MPI_Get_Count allows us to know the len of the message
  int numberAmount = 0;
  MPI_Get_count(&tmpStatus, MPI_BYTE, &numberAmount);
  //Create buffer to store in-coming data
  char buffer[numberAmount];
  //Create new Status (pretty sure overwriting the probe one would be good aswell)
  MPI_Status realStatus;
  //Receive the message who was scouted
  MPI_Recv(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, tmpStatus.MPI_SOURCE, tmpStatus.MPI_TAG, MPI_COMM_WORLD, &realStatus);
  lastTimer = clock();
  if (realStatus.MPI_TAG < 0)
  {
    if (term <= realStatus.MPI_TAG * -1)
    {
      state = Status::Follower;
      leaderRank = realStatus.MPI_SOURCE;
      //Should followers answer to the leader? #TODO DISCUSS ME (I think not)
    }
  }
  else if (realStatus.MPI_TAG > 0) //FIXME if TAG happens to be 0 then a term++ is missing in the code;
  {
    //Vote demande from a server.
    if (strncmp(buffer, "v_", 2) == 0 && realStatus.MPI_SOURCE < numberOfNodes)
    {
        if (strcmp(buffer, "v_voteMe") == 0)
        {
          char* msg = "v_NO";
          if (realStatus.MPI_TAG >= term)
          {
            msg = "v_OK";
          }
          MPI_Send(msg, sizeof(msg)/sizeof(char), MPI_BYTE, realStatus.MPI_SOURCE, term, MPI_COMM_WORLD);
        }
        else if (strcmp(buffer, "v_OK") == 0)
        {
          *electionResults[realStatus.MPI_SOURCE] = 2;
        }
        else if (strcmp(buffer, "v_NO") == 0)
        {
          *electionResults[realStatus.MPI_SOURCE] = 1;
        }
        else
        {
          std::cerr<< "Messager eceived with the wrong format"<<std::endl;
        }

    }
    //Demande Client originale ou relayée.
    else if (realStatus.MPI_SOURCE >= numberOfNodes || strncmp(buffer, "r_", 2) == 0)
    {
      if (strncmp(buffer, "r_", 2) != 0)
      {
        char newBuffer[strlen(buffer) + 3] = "r_";
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
      lastWrittenTerm++;
      term++;
    }
  }

}

void Server::handleRequest(char* buffer, int tag)
{
  //1 Est ce déjà en Ram si oui => 4 sinon => 2
  bool existInRam = false;
  char* pureBuffer = buffer+2;
  for (int i = lastWrittenTerm; i < term; i++)
  {
    if (currentLog.at(tag) != pureBuffer)
    {
      existInRam = true;
      break;
    }
  }
  if (!existInRam)
  {
    //2 Mettre en RAM => 3
    currentLog.push_back(std::move(pureBuffer));
    term++;
  }


  //3 Envoyez à tous les nodes et préparer un tableau de retour sinon
  if (term == tag && state == Status::Leader)
  {
    waiting = true;
    for (int i = 0; i < Server::numberOfNodes; i++)
    {
      majorityResults[i] = 0;
      if (i == rank)
      {
        continue;
      }

      MPI_Send(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, i, term, MPI_COMM_WORLD);
    }
  }
  else if (tag < term && state == Status::Leader)
  {
    int agreed = 0;
    int denied = 0;
    *majorityResults[tag] = strcmp(buffer, "r_agreed") ? 2 : 1;

    for (int i = 0; i < numberOfNodes;i++)
    {
      yesBallots += (*majorityResults[i] == 2) ? 1 : 0;
      noBallots += (*majorityResults[i] == 1) ? 1 : 0;
      if (yesBallots > numberOfNodes / 2)
      {
        waiting = false;
        char toSend[strlen(pureBuffer) + 3] = "A_";
        strcat(toSend, pureBuffer);
        term++;
        for (int i = 0; i < Server::numberOfNodes; i++)
        {
          majorityResults[i] = 0;
          if (i == rank)
          {
            continue;
          }

          MPI_Send(toSend, sizeof(toSend)/sizeof(char), MPI_BYTE, i, term, MPI_COMM_WORLD);
        }
        //Envoi d'append Entry plus écriture sur disque
        if (state == Status::Leader)
        {
          //Write on server Disc
          if (term == tag && strcmp(currentLog.at(term), pureBuffer) == 0)
          {
            Server::log << pureBuffer << std::endl;
            lastWrittenTerm++;
          }
          else
          {
            //TODO INITIATE CLEVER ROLLBACK. !!!!! Will impact MPI_send. [Don't forget to increment termof Leader]
          }

          //Propagate append Entry to other nodes.
          lastTimer = clock();
          //Send an heartbeat to every Node except hisself
        }
      }
    }
  }
  else if (leaderRank < 0)
  {
    MPI_Send(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, leaderRank, term, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Send(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, leaderRank, std::rand()%numberOfNodes, MPI_COMM_WORLD);
  }
}

bool Server::update()
{
  //FIXME
  while(true)
  {
    if (state != Status::Leader && clock() - this->lastTimer > this->timeout)
    {
      leaderRank = -1;
      if (state  == Status::Follower)
      {
        startElection();
      }
    }
    if (state == Status::Leader && clock() - this->lastTimer > INITIAL_HEARTBEAT)
    {
      sendHeartbeat();
    }
    receiveMessage();
    if (!waiting && state == Status::Leader)
    {
      waiting = true;
      char* buffer= currentLog[lastWrittenTerm + 1];
      for (int i = 0; i < Server::numberOfNodes; i++)
      {
        majorityResults[i] = 0;
        if (i == rank)
        {
          continue;
        }

        MPI_Send(buffer, sizeof(buffer)/sizeof(char), MPI_BYTE, i, lastWrittenTerm + 1, MPI_COMM_WORLD);
      }
    }
  }
  return 0;
}
