
class NetworkDevice:
    
    def __init__(self, hostname, os_info, interfaces):
        self.hostname = hostname
        self.os_info = os_info
        self.interfaces = interfaces
    
    def __str__(self):
        return "Host: {hostname} (OS: {os_info})"

    def get_info(self):
        return { 'hostname':self.get_os, 'get_os_info':self.get_os_info, 'interfaces':self.interfaces }


class Router(NetworkDevice):
    def __init__(self, hostname, os_info, interfaces, routing_table):
        super().__init__(self, hostname, os_info, interfaces)
        self.routing_table = routing_table

    def get_info(self):
        return { 'hostname':self.get_os, 'get_os_info':self.get_os_info, 'interfaces':self.interfaces, 'routing_table':self.routing_table }
    
    
    def show_routing_table():
        print('')
