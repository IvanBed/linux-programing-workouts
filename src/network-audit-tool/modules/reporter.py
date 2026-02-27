import device as dev
import csv

def save_to_text(devices, filename):
    with open(filename) as file:
        file.write('-------- Devices info --------')
        for device in devices:
            file.write(device.get_info())

def write_to_csv(file_name, devices):
    with open(file_name, mode = 'w', newline = '') as csv_file:
        csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        csv_writer.writeheader()
        for device in devices:
            csv_writer.writerows(device.get_info())
