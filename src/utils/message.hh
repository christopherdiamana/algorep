#pragma once

#include <string>


class Message {

  public:
    ///Constructor
    Message(int from, int dest, int term);
    Message(std::string serial_string);
    ~Message();

    std::string Serialize();

    friend std::ostream& operator << ( std::ostream&, Message& );

  private:
    // Process requesting
    const int from;
    // Process dest
    const int dest;
    // Current epoch
    const int term;
};

class RequestVote : public Message {

  public:
    /**
     * @brief Request a vote, sent from the candiate
     * @param from
     * @param dest
     * @param term Epoch of the sender
     * @param lastLogIndex last log index of the sender
     * @param lastLogTerm last log term of the sender
     */
    ///Constructor
    RequestVote(int from, int dest, int term, int lastLogIndex, int lastLogTerm);
    RequestVote(std::string serial_string);
    ~RequestVote();

    std::string Serialize();

    friend std::ostream& operator << ( std::ostream&, Message& );

  private:
    /// index of candidate's last log entry
    const int lastLogIndex;
    /// term of candidate's last log entry
    const int lastLogTerm;
};


class ReplyVote : public Message {

  public:
    /**
     * @brief Response to RequestVote sent by the candidate
     * @param from
     * @param dest
     * @param term
     * @param granted
     */
     ////Constructor
    ReplyVote(int from, int dest, int term, bool granted);
    ReplytVote(std::string serial_string);
    ~ReplyVote();

    std::string Serialize();

    friend std::ostream& operator << ( std::ostream&, Message& );

  private:
    /// true means candidate obtain the vote
    const bool granted;
};


class AppendEntriesRequest : public Message {

  public:
    AppendEntriesRequest(
        int from,
        int dest,
        int term,
        int prevIndex,
        int prevTerm,
        std::vector<std::string> entries,
        int commitIndex);
    AppendEntriesRequest(std::string serial_string);
    ~AppendEntriesRequest();

    std::string Serialize();

    friend std::ostream& operator << ( std::ostream&, Message& );

  private:
    // index of log entry
    const int prevIndex;
    // Epoch of prevIndex
    const int prevTerm;
    // log entries to store (empty for heartbeat may send more than one for efficiency)
    std::vector<Entry> entries_;
    // leader's commit_index
    const int leader_commit_;
};

class AppendEntriesReply : public Message {

  public:
    AppendEntriesReply(int from, int dest, int term, bool success, int matchIndex);
    AppendEntriesReply(std::string serial_string);
    ~AppendEntriesReply();

    std::string Serialize();
    friend std::ostream& operator << ( std::ostream&, Message& );

  private:
    const bool success;
    const int matchIndex;
}
