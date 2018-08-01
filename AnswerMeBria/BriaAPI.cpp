#include "BriaAPI.h"

BriaAPI::BriaAPI()
{

}

BriaAPI::~BriaAPI()
{

}

// sends a message to bria asking for the current call status
// if a reply was recieved returns 0 and responce is brias reply
// if not returns the return code.
 int BriaAPI::GetCallStatus(std::string &responce, Pipe& pipe) {

	 pipe.PipeMessage(Pipe::ConstructMessage("status", "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<status><type>call</type></status>"));
	 do {// keep checking the pipe until you get the proper responce
		 responce = pipe.RecieveMessage();
	 } while (responce.find("<status type=\"call\">") == std::string::npos);

	 return 0;
}

 // answer the given caller id
 int BriaAPI::AnswerCall(Pipe &pipe, std::string id, std::string& responce) {
	 pipe.PipeMessage(Pipe::ConstructMessage("answer", "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<answerCall><callId>" + id + "</callId></answerCall>"));
	 do {// keep checking the pipe until you get the proper responce
		 responce = pipe.RecieveMessage();
	 } while (responce.find("HTTP/1.1 200 OK") == std::string::npos);
	 return 0;
 }

 // checks the string if it has a call that is ringing
 bool BriaAPI::HasRingingCall(std::string& responce) {
	 if (responce.find("ringing") != -1) {
		 return true;
	 }
	 return false;
 }

 // gets the caller id from the string
 int BriaAPI::GetCallerId(std::string &responce, std::string &callerId) {
	 int idStart = responce.find("<id>") + 4;
	 int idEnd = responce.find("</id>");
	 callerId = responce.substr(idStart, (idEnd - idStart));
	 return 0;
 }