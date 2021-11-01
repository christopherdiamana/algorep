#include "message.hh"


Message::Message(int from, int dest, int term)
    : from(from)
    , dest(dest)
    , term(term)
{}

RequestVote::RequestVote(int from, int dest, int term, int lastLogIndex, int lastLogTerm)
    : Message(from, dest, term)
    , lastLogIndex(lastLogIndex)
    , lastLogTerm(lastLogTerm)
{}

ReplyVote::ReplyVote(int from, int dest, int term, bool granted)
    : Message(from, dest, term)
    , granted(granted)
{}


AppendEntriesRequest::AppendEntriesRequest(
    int from, int dest, int term, int prevIndex, int prevTerm,
    std::vector<std::string> entries, int commitIndex)
    : Message(from, dest, term)
    , prevIndex(prevIndex)
    , prevTerm(prevTerm)
    , entries(entries)
    , commitIndex(commitIndex)
{}

AppendEntriesReply::AppendEntriesReply(
    int from, int dest, int term, bool success, int matchIndex)
    : Message(from, dest, term)
    , success(success)
    , matchIndex(matchIndex)
{}

