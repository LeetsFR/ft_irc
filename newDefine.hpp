#define RPL_NAMREPLY(clientNick, channel, userlist) "353 " + clientNick + " = " + channel + " :" + userlist + "\r\n"
#define PRIVMSG(sender, target, message) ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n"
#define RPL_ENDOFNAMES(clientNick, channel) "366 " + clientNick + " " + channel + " :End of /NAMES list\r\n"
#define RPL_NOTOPIC(clientNick, channelName) ("331 " + clientNick + " " + channelName + " :No topic is set\r\n")
#define RPL_TOPIC(clientNick, channel, topic) ("332 " + clientNick + " " + channel + " :" + topic + "\r\n")
#define REP_CAPEND(clientNick) "001 " + clientNick + " :Welcome to the Internet Relay Network " + clientNick + "\r\n"
#define REP_PONG(serverName) (string("PONG ") + serverName + "\r\n")
#define ERR_NEEDMOREPARAMS(clientNick, command) "461 " + clientNick + " " + command + " :Not enough parameters\r\n"
#define ERR_PASSWDMISMATCH(clientNick) "464 " + clientNick + " :Password incorrect\r\n"
#define ERR_NOORIGIN(clientNick) "409 " + clientNick + " :No origin specified\r\n"
#define ERR_NONICKNAMEGIVEN(clientNick) "431 " + clientNick + " :No nickname given\r\n"
#define ERR_NICKNAMEINUSE(clientNick, nick) "433 " + clientNick + " " + nick + " :Nickname is already in use\r\n"
#define ERR_ERRONEUSNICKNAME(clientNick, nick) "432 " + clientNick + " " + nick + " :Erroneous nickname\r\n"
#define ERR_CANNOTSENDTOCHAN(clientNick, channel) "404 " + clientNick + " " + channel + " :Cannot send to channel\r\n"
#define ERR_NOTEXTTOSEND(clientNick) "412 " + clientNick + " :No text to send\r\n"
#define ERR_INPUTTOOLONG(clientNick) "417 " + clientNick + " :Input line was too long\r\n"
#define ERR_NOSUCHCHANNEL(clientNick, channel) "403 " + clientNick + " " + channel + " :No such channel\r\n"
#define ERR_TOOMANYCHANNELS(clientNick, channel) "405 " + clientNick + " " + channel + " :You have joined too many channels\r\n"
#define ERR_BADCHANNELKEY(clientNick, channel) ("475 " + clientNick + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_BANNEDFROMCHAN(clientNick, channel) "474 " + clientNick + " " + channel + " :Cannot join channel (+b)\r\n"
#define ERR_CHANNELISFULL(clientNick, channel) "471 " + clientNick + " " + channel + " :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN(clientNick, channel) ("473 " + clientNick + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(clientNick, channel) "476 " + clientNick + " " + channel + " :Bad channel mask\r\n"
#define ERR_CHANOPRIVSNEEDED(clientNick, channel) "482 " + clientNick + " " + channel + " :You're not channel operator\r\n"
#define ERR_USERNOTINCHANNEL(clientNick, nick, channel) ("441 " + clientNick + " " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(clientNick, channel) "442 " + clientNick + " " + channel + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(nick, channel) ("443" + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(clientNick, nick) "401 " + clientNick + " " + nick + " :No such nick\r\n"
#define RPL_INVITING(clientNick, channelName, targetNick) "341 " + clientNick + " " + channelName + " " + targetNick + "\r\n"

