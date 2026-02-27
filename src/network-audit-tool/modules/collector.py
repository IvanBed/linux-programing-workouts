import re
import platform 
import subprocess

def get_hostname():
    pass

def get_os_info():
    return platform.system() + ' ' + platform.release()

def get_ip_addresses():
    pass

def get_routing_table():
    pass

def get_next_el(arr_list, word):
    try:
        idx = arr_list.index(word)
        return False, arr_list[idx + 1] 
    except:
        return True, None

def del_empty(tokens):
    for token in tokens:
        if token == '' or token is None :
            tokens.remove(token)
        else:
            token = token.strip()
    return tokens

def get_network_interfaces_info(ip_data):
   
    if ip_data == '':
        return True, None
    
    network_interfaces = []
    
    rows_pattern = r'^[1-9]+:|\n[1-9]+:'
    
    tokens = del_empty(re.split(rows_pattern, ip_data))
            
    for token in tokens:
        network_interface = {}
        words = del_empty(token.split(' '))

        network_interface['name'] = words[0].strip()
        
        err, mac = get_next_el(words, 'link/ether')
        if not err:
            network_interface['link/ether'] = mac
        else:
            network_interface['link/ether'] = 'No mac address for that interface'
        
        err, ipv4 = get_next_el(words, 'inet')
        if not err:
            network_interface['inet'] = ipv4
        else:
            network_interface['inet'] = 'No ipv4 for that interface'

        network_interfaces.append(network_interface)
    
    return False, network_interfaces
