import collector
import device
import reporter

def get_device() -> device.NetworkDevice:
    system_info = collector.get_system_info()
    device = None
    if system_info['routing_table_info'] == None:
        device = device.NetworkDevice(system_info['hostname'], system_info['os_info'], system_info['interfaces_info'])
    else: 
        device = device.NetworkDevice(system_info['hostname'], system_info['os_info'], system_info['interfaces_info'], system_info['routing_table_info'])

    return device

def main():
    device = get_device() 
    reporter.save_to_text(device, 'device_info')

if __name__ == 'main':
    main()

