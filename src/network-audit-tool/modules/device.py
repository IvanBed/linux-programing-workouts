import collector

class NetworkDevice:
    
    _hostname = '' 
    _os_info = ''
    _interfaces = ''

    def __init__(self):
        self.hostname   = collector.get_hostname()
        self.ip_address = collector.get_os_info()
        self.interfaces = collector.get_ip_addresses()
    
    def get_info(self):
        return { 'hostname':self.get_os, 'get_os_info':self.get_os_info, 'get_ip_addresses':self.get_ip_addresses }


class Router(NetworkDevice):
    def __init__(self):
        super().__init__(self)
        self.routing_table = collector.get_routing_table()

    def get_info(self):
        return { 'hostname':self.get_os, 'get_os_info':self.get_os_info, 'get_ip_addresses':self.get_ip_addresses, 'routing_table':self.routing_table }
    
    
    def show_routing_table():
        print('')


class Switch(NetworkDevice):
    def __init__(self):
        super().__init__(self)

    def get_info(self):
        return { 'hostname':self.get_os, 'get_os_info':self.get_os_info, 'get_ip_addresses':self.get_ip_addresses}
    
    
    def show_routing_table():
        print('')