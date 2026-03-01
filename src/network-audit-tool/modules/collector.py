import re
import platform 
import subprocess

def get_hostname() -> str:
    pass

def get_os_info() -> str:
    return platform.system() + ' ' + platform.release()

def get_ip_addresses():
    pass

def get_next_el(arr_list: list[str], word: str) -> tuple[bool, str]:
    try:
        idx = arr_list.index(word)
        return False, arr_list[idx + 1] 
    except:
        return True, None

def del_empty(tokens: list[str]) -> list[str]:
    for token in tokens:
        if token == '' or token is None :
            tokens.remove(token)
        else:
            token = token.strip()
    return tokens

def get_network_interfaces_info(ip_data: str) -> tuple[bool, list[dict[str, str]]]:
   
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


def get_routing_table_info(routes_data: str) -> tuple[bool, list[dict[str, str]]]:
    
    if routes_data == '':
        return True, None
    
    routing_table_entities = []
    rows = del_empty(routes_data.splitlines())
    for row in rows:
        tokens = row.split(' ')
        routing_table_entity = {}
        
        routing_table_entity['network'] = tokens[0]
        err, gateway = get_next_el(tokens, 'via')
        if not err:
            routing_table_entity['gateway'] = gateway
        else:
            routing_table_entity['gateway'] = ' '
       
        err, src = get_next_el(tokens, 'src')
        if not err:
            routing_table_entity['src'] = src
        else:
            routing_table_entity['src'] = ' '
            
        err, interface_type = get_next_el(tokens, 'dev')
        if not err:
            routing_table_entity['interface_type'] = interface_type
        else:
            routing_table_entity['interface_type'] = ' '
        
        err, protocol = get_next_el(tokens, 'proto')
        if not err:
            routing_table_entity['protocol'] = protocol
        else:
            routing_table_entity['protocol'] = ' '
        
        err, scope = get_next_el(tokens, 'scope')
        if not err:
            routing_table_entity['scope'] = scope
        else:
            routing_table_entity['scope'] = ' '
            
        routing_table_entities.append(routing_table_entity)    
        
    
    return False, routing_table_entities