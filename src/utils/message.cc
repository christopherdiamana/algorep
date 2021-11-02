#include "message.hh"


Message::Message(int from, int dest, int term)
    : from(from)
    , dest(dest)
    , term(term)
{}

Message::Message(std::string serial_string) {
  std::string delimiter = " ";
  size_t pos = 0;
  std::string token;
  std::string tokens[3];
  int i = 0;

  while ((pos = serial_string.find(delimiter)) != std::string::npos) {
    token = serial_string.substr(0, pos);
    tokens[i] = token;
    serial_string.erase(0, pos + delimiter.length());
    i++;
  }
  tokens[i] = serial_string;

  this->from = std::stoi(tokens[0]);
  this->dest = std::stoi(tokens[1]);
  this->term = std::stoi(tokens[2]);
}

std::string Message::Serialize() {
  const char DELIMITER = ' ';
  std::stringstream s;
  s << this->from << DELIMITER << this->dest << DELIMITER << this->term;
  return s.str();
}


RequestVote::RequestVote(int from, int dest, int term, int lastLogIndex, int lastLogTerm)
    : Message(from, dest, term)
    , lastLogIndex(lastLogIndex)
    , lastLogTerm(lastLogTerm)
{}

RequestVote::RequestVote(std::string serial_string) {
  std::string delimiter = " ";
  size_t pos = 0;
  std::string token;
  std::string tokens[5];
  int i = 0;

  while ((pos = serial_string.find(delimiter)) != std::string::npos) {
    token = serial_string.substr(0, pos);
    tokens[i] = token;
    serial_string.erase(0, pos + delimiter.length());
    i++;
  }
  tokens[i] = serial_string;

  this->from = std::stoi(tokens[0]);
  this->dest = std::stoi(tokens[1]);
  this->term = std::stoi(tokens[2]);
  this->lastLogIndex = std::stoi(tokens[3]);
  this->lastLogTerm = std::stoi(tokens[4]);
}

std::string RequestVote::Serialize() {
  const char DELIMITER = ' ';
  std::stringstream s;
  s << this->from << DELIMITER
    << this->dest << DELIMITER
    << this->term << DELIMITER
    << this->lastLogIndex << DELIMITER
    << this->lastLogTerm;
  return s.str();
}


ReplyVote::ReplyVote(int from, int dest, int term, bool granted)
    : Message(from, dest, term)
    , granted(granted)
{}

ReplyVote::ReplyVote(std::string serial_string) {
  std::string delimiter = " ";
  size_t pos = 0;
  std::string token;
  std::string tokens[4];
  int i = 0;

  while ((pos = serial_string.find(delimiter)) != std::string::npos) {
    token = serial_string.substr(0, pos);
    tokens[i] = token;
    serial_string.erase(0, pos + delimiter.length());
    i++;
  }
  tokens[i] = serial_string;

  this->from = std::stoi(tokens[0]);
  this->dest = std::stoi(tokens[1]);
  this->term = std::stoi(tokens[2]);
  std::istringstream(tokens[3]) >> std::boolalpha >> this->lastLogTerm;
}

std::string ReplyVote::Serialize() {
  const char DELIMITER = ' ';
  std::stringstream s;
  s << this->from << DELIMITER
    << this->dest << DELIMITER
    << this->term << DELIMITER
    << this->granted;
  return s.str();
}

AppendEntriesRequest::AppendEntriesRequest(
    int from, int dest, int term, int prevIndex, int prevTerm,
    std::vector<std::string> entries, int commitIndex)
    : Message(from, dest, term)
    , prevIndex(prevIndex)
    , prevTerm(prevTerm)
    , entries(entries)
    , commitIndex(commitIndex)
{}

AppendEntriesRequest::AppendEntriesRequest(std::string serial_string) {
  std::string delimiter = " ";
  size_t pos = 0;
  std::string token;
  std::string tokens[6];
  int i = 0;

  while ((pos = serial_string.find(delimiter)) != std::string::npos) {
    token = serial_string.substr(0, pos);
    tokens[i] = token;
    serial_string.erase(0, pos + delimiter.length());
    i++;
  }
  tokens[i] = serial_string;

  this->from = std::stoi(tokens[0]);
  this->dest = std::stoi(tokens[1]);
  this->term = std::stoi(tokens[2]);
  this->prevIndex = std::stoi(tokens[3]);
  this->prevTerm = std::stoi(tokens[4]);
  this->entries = std::stoi(tokens[5]); // FIXME
  this->commitIndex = std::stoi(tokens[6]);
}

std::string AppendEntriesRequest::Serialize() {
  const char DELIMITER = ' ';
  std::stringstream s;
  s << this->from << DELIMITER
    << this->dest << DELIMITER
    << this->term << DELIMITER
    << this->prevIndex << DELIMITER
    << this->prevTerm << DELIMITER
    << this->entries << DELIMITER //FIXME
    << this->commitIndex;
  return s.str();
}


AppendEntriesReply::AppendEntriesReply(
    int from, int dest, int term, bool success, int matchIndex)
    : Message(from, dest, term)
    , success(success)
    , matchIndex(matchIndex)
{}

AppendEntriesReply::AppendEntriesReply(std::string serial_string) {
  std::string delimiter = " ";
  size_t pos = 0;
  std::string token;
  std::string tokens[5];
  int i = 0;

  while ((pos = serial_string.find(delimiter)) != std::string::npos) {
    token = serial_string.substr(0, pos);
    tokens[i] = token;
    serial_string.erase(0, pos + delimiter.length());
    i++;
  }
  tokens[i] = serial_string;

  this->from = std::stoi(tokens[0]);
  this->dest = std::stoi(tokens[1]);
  this->term = std::stoi(tokens[2]);
  std::istringstream(tokens[3]) >> std::boolalpha >> this->success;
  this->matchIndex = std::stoi(tokens[4]);
}

std::string AppendEntriesReply::Serialize() {
  const char DELIMITER = ' ';
  std::stringstream s;
  s << this->from << DELIMITER
    << this->dest << DELIMITER
    << this->term << DELIMITER
    << this->lastLogIndex << DELIMITER
    << this->lastLogTerm;
  return s.str();
}

