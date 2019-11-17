#include <string>
#include <stdio.h>

#include "bt_transmitter.hpp"

// Initialize rfcomm file name path
// and MAC address of the device to
// connect with.
BTTransmitter::BTTransmitter(
	std::string port_name,
	std::string bt_mac_addr) 
{
	BTTransmitter::_port_name = port_name;
	BTTransmitter::_bt_mac_addr = bt_mac_addr;
}

bool BTTransmitter::connect() 
{
	// Create CLI command to run.
	// This will associate the MAC address of the partner
	// device with a file acting as a serial port for bluetooth.
	std::string command = "rfcomm connect " + BTTransmitter::_port_name + " " 
		+ BTTransmitter::_bt_mac_addr + "&";	

	// Run CLI command.
	int status  = system(command.c_str());
	if (status == 0) 
	{
		return true;
	}
	else 
	{
		return false;
	}

}

bool BTTransmitter::send(char message) 
{
	std::string port_name = BTTransmitter::_port_name;
	FILE *serPort = fopen(port_name.c_str(), "w");

	if (serPort == NULL) 
	{
		return false;
	}	

	char writeBuffer[] = {message};

	fwrite(writeBuffer, sizeof(char), sizeof(writeBuffer), serPort);
	fclose(serPort);

	return true;
}

