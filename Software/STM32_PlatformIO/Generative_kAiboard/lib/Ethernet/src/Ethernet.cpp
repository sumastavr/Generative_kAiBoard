/* Copyright 2018 Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Arduino.h>
#include "Ethernet.h"
//#include "utility/w5100.h" //TODO: path Check
#include "w5100.h"
#include "Dhcp.h"

IPAddress EthernetClass::_dnsServerAddress;
DhcpClass* EthernetClass::_dhcp = NULL;

int EthernetClass::begin(uint8_t *mac, unsigned long timeout, unsigned long responseTimeout)
{
	static DhcpClass s_dhcp;
	_dhcp = &s_dhcp;

	// Initialise the basic info
	if (W5300.init() == 0) return 0;

	W5300.setMACAddress(mac);
	W5300.setIPAddress(IPAddress(0,0,0,0).raw_address());

	// Now try to get our config info from a DHCP server
	int ret = _dhcp->beginWithDHCP(mac, timeout, responseTimeout);
	if (ret == 1) {
		// We've successfully found a DHCP server and got our configuration
		// info, so set things accordingly

		W5300.setIPAddress(_dhcp->getLocalIp().raw_address());
		W5300.setGatewayIp(_dhcp->getGatewayIp().raw_address());
		W5300.setSubnetMask(_dhcp->getSubnetMask().raw_address());

		_dnsServerAddress = _dhcp->getDnsServerIp();
		socketPortRand(micros());
	}
	return ret;
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip)
{
	// Assume the DNS server will be the machine on the same network as the local IP
	// but with last octet being '1'
	IPAddress dns = ip;
	dns[3] = 1;
	begin(mac, ip, dns);
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip, IPAddress dns)
{
	// Assume the gateway will be the machine on the same network as the local IP
	// but with last octet being '1'
	IPAddress gateway = ip;
	gateway[3] = 1;
	begin(mac, ip, dns, gateway);
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway)
{
	IPAddress subnet(255, 255, 255, 0);
	begin(mac, ip, dns, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet)
{
	if (W5300.init() == 0) return;

	W5300.setMACAddress(mac);
	W5300.setIPAddress(ip._address.bytes);
	W5300.setGatewayIp(gateway._address.bytes);
	W5300.setSubnetMask(subnet._address.bytes);

	//Serial.println("Ethernet::begin(), W5300 IP setup");
	_dnsServerAddress = dns;
}

void EthernetClass::init(uint8_t sspin)
{
	W5300.setSS(sspin);  //TODO: W5300, check sspin
}

EthernetLinkStatus EthernetClass::linkStatus()
{
	switch (W5300.getLinkStatus()) {
		case UNKNOWN:  return Unknown;
		case LINK_ON:  return LinkON;
		case LINK_OFF: return LinkOFF;
		default:       return Unknown;
	}
}

EthernetHardwareStatus EthernetClass::hardwareStatus()
{
  uint16_t chipId = W5300.getChipId();
  if(chipId == 0x53) chipId = 53;

  switch (chipId) {
		case 51: return EthernetW5100;
		case 52: return EthernetW5200;
		case 53: return EthernetW5300;
		case 55: return EthernetW5500;
		default: return EthernetNoHardware;
	}
}

int EthernetClass::maintain()
{
	int rc = DHCP_CHECK_NONE;
	if (_dhcp != NULL) {
		// we have a pointer to dhcp, use it
		rc = _dhcp->checkLease();
		switch (rc) {
		case DHCP_CHECK_NONE:
			//nothing done
			break;
		case DHCP_CHECK_RENEW_OK:
		case DHCP_CHECK_REBIND_OK:
			//we might have got a new IP.
			W5300.setIPAddress(_dhcp->getLocalIp().raw_address());
			W5300.setGatewayIp(_dhcp->getGatewayIp().raw_address());
			W5300.setSubnetMask(_dhcp->getSubnetMask().raw_address());
			_dnsServerAddress = _dhcp->getDnsServerIp();
			break;
		default:
			//this is actually an error, it will retry though
			break;
		}
	}
	return rc;
}


void EthernetClass::MACAddress(uint8_t *mac_address)
{
	W5300.getMACAddress(mac_address);
}

IPAddress EthernetClass::localIP()
{
	IPAddress ret;
	W5300.getIPAddress(ret.raw_address()); //W5300 Debug
	return ret;
}

IPAddress EthernetClass::subnetMask()
{
	IPAddress ret;
	W5300.getSubnetMask(ret.raw_address());
	return ret;
}

IPAddress EthernetClass::gatewayIP()
{
	IPAddress ret;
	W5300.getGatewayIp(ret.raw_address());
	return ret;
}

void EthernetClass::setMACAddress(const uint8_t *mac_address)
{
	W5300.setMACAddress(mac_address);
}

void EthernetClass::setLocalIP(const IPAddress local_ip)
{
	IPAddress ip = local_ip;
	W5300.setIPAddress(ip.raw_address());
}

void EthernetClass::setSubnetMask(const IPAddress subnet)
{
	IPAddress ip = subnet;
	W5300.setSubnetMask(ip.raw_address());
}

void EthernetClass::setGatewayIP(const IPAddress gateway)
{
	IPAddress ip = gateway;
	W5300.setGatewayIp(ip.raw_address());
}

void EthernetClass::setRetransmissionTimeout(uint16_t milliseconds)
{
	if (milliseconds > 6553) milliseconds = 6553;
	W5300.setRetransmissionTime(milliseconds * 10);
}

void EthernetClass::setRetransmissionCount(uint8_t num)
{
	W5300.setRetransmissionCount(num);
}

EthernetClass Ethernet;
