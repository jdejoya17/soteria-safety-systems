#ifndef BT_TRANSMITTER_H
#define BT_TRANSMITTER_H

#include <string>

class BTTransmitter 
{
	private:
		std::string _port_name;
		std::string _bt_mac_addr;

	public:
		BTTransmitter(std::string port_name, std::string bt_mac_addr);
		bool connect();
		bool send(char message);

};



#endif
