#include "Shout.h"

Define_Module(Shout);

Shout::Shout() : initiator(-1), root(false), parent(-1), counter(0) { 
  status = Status::IDLE; 
}

void Shout::initialize() {
  if (getIndex() == par("initiator").intValue()) {
    timer = new omnetpp::cMessage("TIMER", MsgKind::TIMER);
    omnetpp::simtime_t startTime = par("startTime");
    scheduleAt(startTime, timer);
    status = Status::INITIATOR;
    EV_INFO << "Node[" << getIndex() << "] is the initiator\n";
  }
}

void Shout::handleMessage(omnetpp::cMessage* recvMsg) {
  if (status == Status::INITIATOR) {
    if (recvMsg->getKind() == MsgKind::TIMER) { //A1
      root = true;
      msg = new omnetpp::cMessage("query", MsgKind::QUERY);
      localBroadcast(msg);
      status = Status::ACTIVE;
    }
    else
      omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
  }
  else if(status == Status::IDLE) {
    if (recvMsg->getKind() == MsgKind::QUERY) {
      parent = recvMsg->getArrivalGate()->getIndex();
      msg = new omnetpp::cMessage("yes", MsgKind::YES);
      send(msg, "port$o", parent);
      counter++;
      if (gateSize("port$o") == counter) status = Status::DONE;
      else {
        localFlooding(recvMsg);
        status = Status::ACTIVE;
      }
    }
    else
      omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
  }
  else if(status == Status::ACTIVE) {
    int senderPort = recvMsg->getArrivalGate()->getIndex();
    if (recvMsg->getKind() == MsgKind::QUERY) {
      recvMsg->setName("no");
      recvMsg->setKind(MsgKind::NO);
      send(recvMsg, "port$o", senderPort);
    }
    else if (recvMsg->getKind() == MsgKind::YES) {
      children.push_back(senderPort);
      counter++;
      delete recvMsg;
      if (counter == gateSize("port$o")) status = Status::DONE;
    }
    else if (recvMsg->getKind() == MsgKind::NO) {
      counter++;
      delete recvMsg;
      if (counter == gateSize("port$o")) status = Status::DONE;
    }
    else
      omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
  }
  else
    omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
}

void Shout::refreshDisplay() const {
  std::string info = status.str();
  info += '\n';
  for (auto& port : children) {
    info += std::to_string(port) + ',';
    gate("port$o", port)->getChannel()->getDisplayString().setTagArg("ls", 0, "red");
  }
  if (parent >= 0)
    gate("port$o", parent)->getChannel()->getDisplayString().setTagArg("ls", 0, "red");
  getDisplayString().setTagArg("t", 0, info.c_str());
}
